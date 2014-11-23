#include <string.h>
#include <stdlib.h>

#include "lpc17xx_gpio.h"

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
//#define DEFAULT_POS_3	1950	// Currently not used
#define DEFAULT_POS_4	1950

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

static xTaskHandle xUARTReceiverTask = NULL, xPWMControllerTask = NULL, xSPIControllerTask = NULL;

static Peripheral_Descriptor_t xUART3 = NULL, xSPIPort = NULL;

xQueueHandle xPWMQueue = 0, xOpCodeQueue = 0, xSPIQueue = 0;

void vUARTStart( void ) {

	xOpCodeQueue = xQueueCreate(4, sizeof( uint8_t ) );
	xPWMQueue = xQueueCreate(1, 4 * sizeof( int8_t ) );
	xSPIQueue = xQueueCreate(1, 4 * sizeof( int8_t ) );

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
			//if( ( ' ' <= cRxedChar ) && ( '~' >= cRxedChar ) ) {
				if( cInputIndex < pwmctrlMAX_INPUT_SIZE ) {
					cInputString[ cInputIndex ] = cRxedChar;
					cInputIndex++;
				}
			//}
		}
	}
}

void prvParseCommand( const int8_t * pcUartInput ) {
	uint8_t ucOpCode, ucSPI, ucCheckSum;
	int8_t sPWM[3];

	ucOpCode = pcUartInput[0];
	sPWM[0] = pcUartInput[1];
	sPWM[1] = pcUartInput[2];
	sPWM[2] = pcUartInput[3];
	ucSPI = pcUartInput[4];

	( void ) ucCheckSum;
//	if ( ucCheckSum == ucOpCode ^ sPWM[0] ^ sPWM[1] ^ sPWM[2] ^ sPWM[3] ^ ucSPI ) {
		xQueueSend( xOpCodeQueue, &ucOpCode, portMAX_DELAY );
		xQueueSend( xPWMQueue, sPWM, portMAX_DELAY );
		xQueueSend( xSPIQueue, &ucSPI, portMAX_DELAY );
//	}
}

void vArmControllerTaskStart( void ) {

	xTaskCreate ( prvPWMController,
				( const int8_t * const ) "PWMCtrl",
				configPWM_CONTROLLER_STACK_SIZE,
				NULL,
				configPWM_CONTROLLER_TASK_PRIORITY,
				&xPWMControllerTask);
}

void prvPWMController( void *prvParameters ) {

	uint8_t cCommand, cServoPosition[5];
	int8_t sParameters[3];

	( void ) prvParameters;

	prvInitPWM();

	for(;;) {
		xQueueReceive( xOpCodeQueue, &cCommand, portMAX_DELAY );

		xQueueReceive( xPWMQueue, sParameters, portMAX_DELAY );

#if pwmctrlDEBUG
			uint16_t buffer[8], i = 0;

			while(i < 3) {
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

		if ( 'r' == cCommand || 'a' == cCommand ) {
			if ( 'r' == cCommand ) {
				LPC_PWM1->MR1 += 8 * sParameters[0];
				LPC_PWM1->MR2 += 8 * sParameters[1];
				LPC_PWM1->MR4 += 8 * sParameters[2];
			} else if ( 'a' == cCommand ) {
				LPC_PWM1->MR1 = 1000 + (8 * sParameters[0] );
				LPC_PWM1->MR2 = 1000 + (8 * sParameters[1] );
				LPC_PWM1->MR4 = 1000 + (8 * sParameters[2] );

			}
			LPC_PWM1->MR1 = ( 2000 < LPC_PWM1->MR1 ) ? 2000 :
							( 1000 > LPC_PWM1->MR1 ) ? 1000 :
							LPC_PWM1->MR1;

			LPC_PWM1->MR2 = ( 1650 < LPC_PWM1->MR2 ) ? 1600 :
							( 1350 > LPC_PWM1->MR2 ) ? 1350 :
							LPC_PWM1->MR2;

			LPC_PWM1->MR4 = ( 1950 < LPC_PWM1->MR4 ) ? 1900 :
							( 1400 > LPC_PWM1->MR4 ) ? 1400 :
							LPC_PWM1->MR4;

			LPC_PWM1->LER = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 4);

		/* The 'p' command return the position of the selected servo. */
		} else if ('p' == cCommand ) {
			if ( sParameters[0] )
				itoa( LPC_PWM1->MR1, cServoPosition, 16);
			else if ( sParameters[1] )
				itoa( LPC_PWM1->MR2, cServoPosition, 16);
			else if ( sParameters[2] )
				itoa( LPC_PWM1->MR4, cServoPosition, 16);
			else
				itoa( 0, cServoPosition, 16);
			if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
				FreeRTOS_write( xUART3, cServoPosition, strlen( cServoPosition ) );
			}
		}
		xQueueReset( xOpCodeQueue );
		xQueueReset( xPWMQueue );
	}
}

void prvInitPWM( void ) {

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
		// LPC_PWM1->MR3 = DEFAULT_POS_3; not used
		LPC_PWM1->MR4 = DEFAULT_POS_4;

		/* Latch the registers to move the servos. */
		LPC_PWM1->LER = 0x27;

		/* Reset PWMMR3, and set interrupts on register 5. */
		LPC_PWM1->MCR = ( 1 << 1 ) | ( 1 << 15 );

		/* Enable the outputs on PWMMR1, PWMMR2, PWMMR3, & PWMMR4. */
		LPC_PWM1->PCR = ( 1 << 9 ) | ( 1 << 10 ) | ( 1 << 11 ) | ( 1 << 12 );

		/* Enable the timer and prescale counters, and enable PWM mode. */
		LPC_PWM1->TCR = (1 << 0) | (1 << 3);
}

void vPlatformControllerTaskStart( void ) {

	xTaskCreate ( prvSPIController,
				( const int8_t * const ) "SPICtrl",
				configSPI_CONTROLLER_STACK_SIZE,
				NULL,
				configSPI_CONTROLLER_TASK_PRIORITY,
				&xSPIControllerTask);
}

void prvSPIController( void *prvParameters ) {
	const uint32_t xClockSpeed = 1000000UL;
	uint8_t ucSpiByte;

	( void ) prvParameters;

	/* Configure the IO pin used for the 7 segment display CS line. */
	GPIO_SetDir( boardSPI_CS_PORT, boardSPI_CS_PIN, boardGPIO_OUTPUT );
	boardSPI_DEASSERT_CS();

	/* Ensure the OLED, which is on the same SSP bus, is not selected. */
	GPIO_SetDir( boardOLED_CS_PORT, boardOLED_CS_PIN, boardGPIO_OUTPUT );
	boardOLED_DEASSERT_CS();


	/* Open the SSP port used for writing to the 7 segment display.  The second
	parameter (ulFlags) is not used in this case.  By default, the SSP will open
	in SPI/polling mode. */
	xSPIPort = FreeRTOS_open( boardSPI_SSP_PORT, ( uint32_t ) pwmctrlPARAMETER_NOT_USED );
	configASSERT( xSPIPort );

	/* Decrease the bit rate a bit, just to demonstrate an ioctl function being
	used.  By default it was 1MHz, this sets it to 500KHz. */
	FreeRTOS_ioctl( xSPIPort, ioctlSET_SPEED, ( void * ) xClockSpeed );

	/* At the time of writing, by default, the SSP will open all its parameters
	set correctly for communicating with the 7-segment display, but explicitly
	set all	parameters anyway, in case the defaults	changes in the future. */
	FreeRTOS_ioctl( xSPIPort, ioctlSET_SPI_DATA_BITS, 		( void * ) boardSSP_DATABIT_8 );
	FreeRTOS_ioctl( xSPIPort, ioctlSET_SPI_CLOCK_PHASE, 	( void * ) boardSPI_SAMPLE_ON_LEADING_EDGE_CPHA_0 );
	FreeRTOS_ioctl( xSPIPort, ioctlSET_SPI_CLOCK_POLARITY, 	( void * ) boardSPI_CLOCK_BASE_VALUE_CPOL_0 );
	FreeRTOS_ioctl( xSPIPort, ioctlSET_SPI_MODE, 			( void * ) boardSPI_MASTER_MODE );
	FreeRTOS_ioctl( xSPIPort, ioctlSET_SSP_FRAME_FORMAT, 	( void * ) boardSSP_FRAME_SPI );

	for( ;; )
	{
		xQueueReceive( xSPIQueue, &ucSpiByte, portMAX_DELAY );

		boardSPI_ASSERT_CS();
		FreeRTOS_write( xSPIPort, &ucSpiByte, sizeof( uint8_t ) );
		boardSPI_DEASSERT_CS();

		xQueueReset( xSPIQueue );
	}
}

