#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IO.h"
#include "queue.h"

#include "pwmctrl.h"

#define pwmctrlDEBUG	1

/* Default positions for the servos */
#define DEFAULT_POS_1	1500
#define DEFAULT_POS_2	1650
#define DEFAULT_POS_3	1900
#define DEFAULT_POS_4	1500

/* Dimensions the buffer into which input characters are placed. */
#define pwmctrlMAX_INPUT_SIZE		50
#define pwmctrlMAX_OUTPUT_SIZE		50

/* Place holder for calls to ioctl that don't use the value parameter. */
#define pwmctrlPARAMETER_NOT_USED	( ( void * ) 0 )

/* Block times of 50 and 500milliseconds, specified in ticks. */
#define pwmctrl50ms					( ( void * ) ( 50UL / portTICK_RATE_MS ) )
#define pwmctrl500ms				( ( void * ) ( 500UL / portTICK_RATE_MS ) )

/* Characters that determine how the commands of UART will be parsed. */
#define pwmctrlSTOP					'\n'
#define pwmctrlDELIM				' '

static xTaskHandle xUARTReceiverTask = NULL, xPWMControllerTask = NULL;

static Peripheral_Descriptor_t xUART3 = NULL;

xQueueHandle xParameterQueue = 0, xCommandQueue = 0;

void vUARTStart( void ) {

	xCommandQueue = xQueueCreate(4, sizeof( uint8_t ) );
	xParameterQueue = xQueueCreate(1, 4 * sizeof( int16_t ) );

	xTaskCreate ( prvUARTReceiver,
				( const int8_t * const ) "UARTRx",
				configUART3_RECEIVER_STACK_SIZE,
				NULL,
				configUART_RECEIVER_TASK_PRIORITY,
				&xUARTReceiverTask);
}

void prvUARTReceiver( void * prvParameters ) {
	int8_t cRxedChar, cInputIndex = 0;
	static int8_t cInputString[ pwmctrlMAX_INPUT_SIZE ];
	portBASE_TYPE xReturned;

	( void ) prvParameters;

	/* Open the UART port used for console input.  The second parameter
	(ulFlags) is not used in this case.  The default board rate is set by the
	boardDEFAULT_UART_BAUD parameter.  The baud rate can be changed using a
	FreeRTOS_ioctl() call with the ioctlSET_SPEED command. */
	xUART3 = FreeRTOS_open( boardUART3, ( uint32_t ) pwmctrlPARAMETER_NOT_USED );
	configASSERT( xUART3 );

	/* Change the Tx usage model from straight polled mode to use zero copy
	buffers with interrupts.  In this mode, the UART will transmit characters
	directly from the buffer passed to the FreeRTOS_write()	function. */
	xReturned = FreeRTOS_ioctl( xUART3, ioctlUSE_ZERO_COPY_TX, pwmctrlPARAMETER_NOT_USED );
	configASSERT( xReturned );

	/* Change the Rx usage model from straight polled mode to use a character
	queue.  Character queue reception is appropriate in this case as characters
	can only be received as quickly as they can be typed, and need to be parsed
	character by character. */
	xReturned = FreeRTOS_ioctl( xUART3, ioctlUSE_CHARACTER_QUEUE_RX, ( void * ) pwmctrlMAX_INPUT_SIZE );
	configASSERT( xReturned );

	/* By default, the UART interrupt priority will have been set to the lowest
	possible.  It must be kept at or below configMAX_LIBRARY_INTERRUPT_PRIORITY,
	but	can be raised above its default priority using a FreeRTOS_ioctl() call
	with the ioctlSET_INTERRUPT_PRIORITY command. */
	xReturned = FreeRTOS_ioctl( xUART3, ioctlSET_INTERRUPT_PRIORITY, ( void * ) ( configMIN_LIBRARY_INTERRUPT_PRIORITY - 1 ) );
	configASSERT( xReturned );

	for(;;) {
		/* Only interested in reading one character at a time. */
		FreeRTOS_read( xUART3, &cRxedChar, sizeof( cRxedChar ) );

#if pwmctrlDEBUG
		if( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
			FreeRTOS_write( xUART3, &cRxedChar, sizeof( cRxedChar ) );
		}
#endif
		/* If we receive the STOP character, stop taking input and parse the input */
		if ( cRxedChar == pwmctrlSTOP ) {
#if pwmctrlDEBUG
			if( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
				FreeRTOS_write( xUART3, "\r", 1 );
			}
#endif
			prvParseCommand( cInputString );

			cInputIndex = 0;
			memset( cInputString, 0x00, pwmctrlMAX_INPUT_SIZE );
		} else {
			if( ( ' ' <= cRxedChar ) && ( '~' >= cRxedChar ) ) {
				if( cInputIndex < pwmctrlMAX_INPUT_SIZE ) {
					cInputString[ cInputIndex ] = cRxedChar;
					cInputIndex++;
				}
			}
		}
	}
}

void prvParseCommand( const int8_t * pcUartInput ) {
	uint8_t ucOpCode, ucSPI, ucCheckSum;
	int16_t sPWM[4];

	ucOpCode = pcUartInput[0];
	sPWM[0] = 16*pcUartInput[1] + pcUartInput[2];
	sPWM[1] = 16*pcUartInput[3] + pcUartInput[4];
	sPWM[2] = 16*pcUartInput[5] + pcUartInput[6];
	sPWM[3] = 16*pcUartInput[7] + pcUartInput[8];
	ucSPI = pcUartInput[9];
	ucCheckSum = ucOpCode ^ sPWM[0] ^ sPWM[1] ^ sPWM[2] ^ sPWM[3] ^ ucSPI;

	xQueueSend( xCommandQueue, &ucOpCode, portMAX_DELAY );
	xQueueSend( xParameterQueue, sPWM, portMAX_DELAY );
}

void vPWMControllerStart( void ) {

	xTaskCreate ( prvPWMController,
				( const int8_t * const ) "PWMCtrl",
				configPWM_CONTROLLER_STACK_SIZE,
				NULL,
				configPWM_CONTROLLER_TASK_PRIORITY,
				&xPWMControllerTask);
}

void prvPWMController( void *prvParameters ) {

	uint8_t cCommand, cServoPosition[5];
	int16_t sParameters[4];

	( void ) prvParameters;

	/* Begin by intitializing the PWM pins. */

	/* Explicitly power the PWM port. It's powered by default, but just
	 * to be safe
	 */
	LPC_SC->PCONP |= ( 1 << 6 );

	/* Reset the timer counter and prescale counter */
	LPC_PWM1->TCR = ( 1 << 1 );

	/* Clear any interrupts on PWM. */
	LPC_PWM1->IR = 0xff;

	/* Set the prescale counter register to have a resolution of 1us. */
	LPC_PWM1->PR = SystemCoreClock / ( 4 * 1000000 ) - 1;

	/* Set pins 2.0, 2.1, 2.2, and 2.3 to operate in PWM mode. */
	LPC_PINCON->PINSEL4 &= ~( 0xf << 0 );
	LPC_PINCON->PINSEL4 |= ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 4 ) | ( 1 << 6 );
	/* Disable pull-up resistors on the PWM pins. */
	LPC_PINCON->PINMODE4 &= ~( 0xff << 0 );
	LPC_PINCON->PINMODE4 |= ( 2 << 0 ) | ( 2 << 2 ) | ( 2 << 4 ) | ( 2 << 6 );

	/* Using a period of 50MHz, with in interrupt occuring 1ms before the next period. */
	LPC_PWM1->MR0 = 20000;
	LPC_PWM1->MR5 = 19000;

	// TODO: Calibarate the physical servos, and determine what to set as their default
	// positions
	/* Upon intializtion, set the servos into their default positions. */
	LPC_PWM1->MR1 = DEFAULT_POS_1;
	LPC_PWM1->MR2 = DEFAULT_POS_2;
	LPC_PWM1->MR3 = DEFAULT_POS_3;
	LPC_PWM1->MR4 = DEFAULT_POS_4;

	/* Latch the registers to move the servos. */
	LPC_PWM1->LER = 0x27;

	/* Reset PWMMR3, and set interrupts on register 5. */
	LPC_PWM1->MCR = ( 1 << 1 ) | ( 1 << 15 );

	/* Enable the outputs on PWMMR1, PWMMR2, PWMMR3, & PWMMR4. */
	LPC_PWM1->PCR = ( 1 << 9 ) | ( 1 << 10 ) | ( 1 << 11 ) | ( 1 << 12 );

	/* Enable the timer and prescale counters, and enable PWM mode. */
	LPC_PWM1->TCR = (1 << 0) | (1 << 3);

	for(;;) {
		xQueueReceive( xCommandQueue, &cCommand, portMAX_DELAY );

		xQueueReceive( xParameterQueue, sParameters, portMAX_DELAY );

#if pwmctrlDEBUG
			uint8_t buffer[8], i = 0;

			while(i < 4) {
				itoa( sParameters[i], buffer, 10);
				if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
					FreeRTOS_write( xUART3, buffer, strlen( buffer ) );
				}
				if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
					FreeRTOS_write( xUART3, "\r\n", strlen( "\r\n" ) );
				}
				i++;
			}
#endif
		/* An 'a' command sets the servo to an absolute position between 1000 and 2000. */
		if ( 'a' == cCommand ) {
			if ( 2000 < sParameters[0] )
				sParameters[0] = 2000;
			else if ( 1000 > sParameters[0] )
				sParameters[0] = 1000;


			if ( 1600 < sParameters[1] )
				sParameters[1] = 1600;
			else if ( 1350 > sParameters[1] )
				sParameters[1] = 1350;

			if ( 1900 < sParameters[2] )
				sParameters[2] = 1900;
			else if ( 1300 > sParameters[2] )
				sParameters[2] = 1400;

			LPC_PWM1->MR1 = sParameters[0];
			LPC_PWM1->MR2 = sParameters[1];
			LPC_PWM1->MR3 = sParameters[2];
//			LPC_PWM1->MR4 = sParameters[3];

		/* An 'r' command moves the servos in a clockwise (positive numbers) or counter-
		clockwise (negative numbers) relative to their current position */
		} else if ('r' == cCommand ) {
			if ( 2000 < LPC_PWM1->MR1 + sParameters[0] )
				LPC_PWM1->MR1 = 2000;
			else if ( 1000 > LPC_PWM1->MR1 + sParameters[0] )
				LPC_PWM1->MR1 = 1000;
			else
				LPC_PWM1->MR1 += sParameters[0];

			if ( 1650 < LPC_PWM1->MR2 + sParameters[0] )
				LPC_PWM1->MR2 = 1650;
			else if ( 1400 > LPC_PWM1->MR2 + sParameters[0] )
				LPC_PWM1->MR2 = 1400;
			else
				LPC_PWM1->MR2 += sParameters[1];

			if ( 1900 < LPC_PWM1->MR3 + sParameters[0] )
				LPC_PWM1->MR3 = 1900;
			else if ( 1300 > LPC_PWM1->MR3 + sParameters[0] )
				LPC_PWM1->MR3 = 1300;
			else
				LPC_PWM1->MR3 += sParameters[2];

//			if ( 2000 < LPC_PWM1->MR4 + sParameters[0] )
//				LPC_PWM1->MR4 = 2000;
//			else if ( 1000 > LPC_PWM1->MR4 + sParameters[0] )
//				LPC_PWM1->MR4 = 1000;
//			else
//				LPC_PWM1->MR4 = sParameters[3];

		/* The 'p' command return the position of the selected servo. */
		} else if ('p' == cCommand ) {
			if ( sParameters[0] )
				itoa( LPC_PWM1->MR1, cServoPosition, 16);
			else if ( sParameters[1] )
				itoa( LPC_PWM1->MR2, cServoPosition, 16);
			else if ( sParameters[2] )
				itoa( LPC_PWM1->MR3, cServoPosition, 16);
			else
				itoa( 0, cServoPosition, 16);
			if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
				FreeRTOS_write( xUART3, cServoPosition, strlen( cServoPosition ) );
			}
		}
		LPC_PWM1->LER = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 );
		xQueueReset( xCommandQueue );
		xQueueReset( xParameterQueue );
	}
}
