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
#define pwmctrlSPI		0

/* Default positions for the servos */
#define DEFAULT_POS_1	1500
#define DEFAULT_POS_2	1350
//#define DEFAULT_POS_3	1950	// Currently not used
#define DEFAULT_POS_4	1050

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

static xTaskHandle xUARTReceiverTask = NULL, xArmControllerTask = NULL, xBaseControllerTask = NULL;

static Peripheral_Descriptor_t xUART3 = NULL;

xQueueHandle xPWMQueue = 0, xOpCodeQueue = 0, xPlatformGPIOQueue = 0;

void vUARTStart( void ) {

	/* Initialize our queues */
	xOpCodeQueue = xQueueCreate(4, sizeof( uint8_t ) );
	xPWMQueue = xQueueCreate(1, 4 * sizeof( int8_t ) );
	xPlatformGPIOQueue = xQueueCreate(1, 4 * sizeof( uint8_t ) );

	/* Start the UART task that will take the command packets */
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
		/* Only interested in reading one character at a time (for now). */
		FreeRTOS_read( xUART3, &cRxedChar, sizeof( cRxedChar ) );

		/* For debugging purposes, echo back each character received to the terminal */
#if pwmctrlDEBUG
		if( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
			FreeRTOS_write( xUART3, &cRxedChar, sizeof( cRxedChar ) );
		}
#endif
		/* If we receive the STOP character, stop taking input and begin parsing the command. */
		if ( cRxedChar == pwmctrlSTOP ) {

		/* Again, for debugging we're just adding a carriage return to make the output look nice. */
#if pwmctrlDEBUG
			if( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
				FreeRTOS_write( xUART3, "\r", 1 );
			}
#endif
			/* Run the method to parse the input received over UART, then reset cInputString to be ready for the next command  */
			prvParseCommand( cInputString );
			cInputIndex = 0;
			memset( cInputString, 0x00, pwmctrlMAX_INPUT_SIZE );

		/* Otherwise, increment the cInputIndex and continue receiving bytes */
		} else {
			if( cInputIndex < pwmctrlMAX_INPUT_SIZE ) {
				cInputString[ cInputIndex ] = cRxedChar;
				cInputIndex++;
			}
		}
	}
}

void prvParseCommand( const int8_t * pcUartInput ) {
	uint8_t ucOpCode, ucGPIO, ucCheckSum;
	int8_t sPWM[3];

	/* Grab each byte from the command string */
	ucOpCode = pcUartInput[0];
	sPWM[0]  = pcUartInput[1];
	sPWM[1]  = pcUartInput[2];
	sPWM[2]  = pcUartInput[3];
	ucGPIO   = pcUartInput[4];

	( void ) ucCheckSum;

	//TODO: implement the checksum, to check for any received errors
//	if ( ucCheckSum == ucOpCode ^ sPWM[0] ^ sPWM[1] ^ sPWM[2] ^ sPWM[3] ^ ucSPI ) {

	/* Send byte 1 to the xOpCodeQueue */
		xQueueSend( xOpCodeQueue, &ucOpCode, portMAX_DELAY );

		/* Send byte 2, 3, and 4 to the PWMQueue, received by the PWM controller */
		xQueueSend( xPWMQueue, sPWM, portMAX_DELAY );

		/* Send byte 5 to the PlatformController Queue */
		xQueueSend( xPlatformGPIOQueue, &ucGPIO, portMAX_DELAY );
//	}
}

void vArmControllerTaskStart( void ) {

	xTaskCreate ( prvArmController,
				( const int8_t * const ) "PWMCtrl",
				configPWM_CONTROLLER_STACK_SIZE,
				NULL,
				configPWM_CONTROLLER_TASK_PRIORITY,
				&xArmControllerTask);
}

void prvArmController( void *prvParameters ) {

	uint8_t cCommand, cServoPosition[5];
	int8_t sParameters[3];

	( void ) prvParameters;

	/* At startup, initialize the pins on the LPC1769 for PWM. */
	prvInitPWM();

	for(;;) {

		/* For for the Op-Code and the PWM position. */
		xQueueReceive( xOpCodeQueue, &cCommand, portMAX_DELAY );
		xQueueReceive( xPWMQueue, sParameters, portMAX_DELAY );

		if ( 'r' == cCommand || 'a' == cCommand ) {

			/* The PWM positions received via the UART receiver allow for control of the servos with a
			 * maximum resolution of 128 per 1000 pulses (the servos bounded by more restricted pulse
			 * width will have, obviously, less). PWM positions are calculated by multiplying the byte
			 * value by 8. For the relative (0x72) command, this number is simply added to the current
			 * value in the match register; for absolute, the new position is calculated as this number
			 * plus 1000 (the minimum pulse width).
			 *
			 * Current position of servo 1: 1500
			 * sParameters[0] = 0x2a
			 * The new position calculated from the 'a' (0x61) command is (8 * 0x2a) + 1000 = 1336.
			 * The new position calcualted form the 'r' (0x72) command is 1500 + (8 * 0x2a) = 1836.
			 *
			 * Note: the parameters bytes can also be negative (e.g., 0xa4), and this would cause a
			 * subtraction from the relative position (no effect on absolute).
			 */

			/* An 'r' command (byte 0x72) move the servo to a new position relative to its current. */
			if ( 'r' == cCommand ) {
				LPC_PWM1->MR1 += 8 * sParameters[0];
				LPC_PWM1->MR2 += 8 * sParameters[1];
				LPC_PWM1->MR4 += 8 * sParameters[2];

			/* An 'a' command (byte 0x61) sets the servo to an absolute position between 1000 and 2000. */
			} else if ( 'a' == cCommand ) {
				LPC_PWM1->MR1 = 1000 + ( 8 * sParameters[0] );
				LPC_PWM1->MR2 = 1000 + ( 8 * sParameters[1] );
				LPC_PWM1->MR4 = 1000 + ( 8 * sParameters[2] );

			}

			/* The following lines check to make sure the servo positions are not going over or under their
			 * safe working positions, and sets them to the maximum or minimum value if this is the case.
			 */
			LPC_PWM1->MR1 = ( 2000 < LPC_PWM1->MR1 ) ? 2000 :
							( 1000 > LPC_PWM1->MR1 ) ? 1000 :
							LPC_PWM1->MR1;

			LPC_PWM1->MR2 = ( 1650 < LPC_PWM1->MR2 ) ? 1650 :
							( 1350 > LPC_PWM1->MR2 ) ? 1350 :
							LPC_PWM1->MR2;

			LPC_PWM1->MR4 = ( 1500 < LPC_PWM1->MR4 ) ? 1500 :
							( 1050 > LPC_PWM1->MR4 ) ? 1050 :
							LPC_PWM1->MR4;

			/* Set the latch enable register to latch MRs 1, 2, and 4 */
			LPC_PWM1->LER = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 4);

		/* The 'p' command return the position of the selected servo, in hex. */
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

		/* Finally, clear the queues and wait for the next command */
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
		/* Upon intialization, set the servos into their default positions. */
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

void vBaseControllerTaskStart( void ) {

	xTaskCreate ( prvBaseController,
				( const int8_t * const ) "GPIOCtrl",
				configSPI_CONTROLLER_STACK_SIZE,
				NULL,
				configSPI_CONTROLLER_TASK_PRIORITY,
				&xBaseControllerTask);
}

void prvBaseController( void *prvParameters ) {

	uint8_t ucPlatformControl;

	( void ) prvParameters;

	LPC_GPIO2->FIODIR |= ( 1 << 6 )  | ( 1 << 7 )  | ( 1 << 8 ); // #1 Forward - Reverse - EN
	LPC_GPIO2->FIODIR |= ( 1 << 10 ) | ( 1 << 11 ) | ( 1 << 12); // #2 Forward - Reverse - EN

	for( ;; ) {
		xQueueReceive( xPlatformGPIOQueue, &ucPlatformControl, portMAX_DELAY );

		/* Send the platform [F]orward by sending forward output to motors 1 and 2. */
		if ( 'F' == ucPlatformControl ) {
			LPC_GPIO2->FIOCLR = ( 1 << 7 );
			LPC_GPIO2->FIOSET = ( 1 << 6 ) | ( 1 << 8 );
			LPC_GPIO2->FIOCLR = ( 1 << 11);
			LPC_GPIO2->FIOSET = ( 1 << 10 ) | ( 1 << 12 );
		}
		/* Send the platform in re[V]erse by sending reverse output to motors 1 and 2. */
		else if ( 'V' == ucPlatformControl ) {
			LPC_GPIO2->FIOCLR = ( 1 << 6 );
			LPC_GPIO2->FIOSET = ( 1 << 7 ) | ( 1 << 8 );
			LPC_GPIO2->FIOCLR = ( 1 << 10);
			LPC_GPIO2->FIOSET = ( 1 << 11 ) | ( 1 << 12 );
		}
		/* Turn the platform [L]eft by forwarding motor 2, reversing motor. 1 */
		else if ( 'L' == ucPlatformControl ) {
			LPC_GPIO2->FIOCLR = ( 1 << 6 );
			LPC_GPIO2->FIOSET = ( 1 << 7 ) | ( 1 << 8 );
			LPC_GPIO2->FIOCLR = ( 1 << 11 );
			LPC_GPIO2->FIOSET = ( 1 << 10 ) | ( 1 << 12 );
		}
		/* Turn the platform [R]ight by forwarding motor 1, reversing motor. 2 */
		else if ( 'R' == ucPlatformControl ) {
			LPC_GPIO2->FIOCLR = ( 1 << 7 );
			LPC_GPIO2->FIOSET = ( 1 << 6 ) | ( 1 << 8 );
			LPC_GPIO2->FIOCLR = ( 1 << 10 );
			LPC_GPIO2->FIOSET = ( 1 << 11 ) | ( 1 << 12 );
		}

		/* [S]top motors 1 & 2, bring the platform to a halt. */
		else if ( 'S' == ucPlatformControl ) {
			LPC_GPIO2->FIOSET = ( 1 << 8 );
			LPC_GPIO2->FIOCLR = ( 1 << 6 ) | ( 1 << 7 );
			LPC_GPIO2->FIOSET = ( 1 << 12 );
			LPC_GPIO2->FIOCLR = ( 1 << 10 ) | ( 1 << 11 );

		/* A 0x00 byte continues the current operation of the motors, anything else is
		 * an invalid command. Clear ENs and wait for next command */
		} else if ( 0x00 != ucPlatformControl ) {
			LPC_GPIO2->FIOCLR = ( 1 << 8 ) | ( 1 << 12 );
		}

		/* Reset the GPIO queue to wait for the next instruction */
		xQueueReset( xPlatformGPIOQueue );
	}
}
