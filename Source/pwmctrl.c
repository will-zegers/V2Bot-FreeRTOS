#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IO.h"
#include "queue.h"

#include "pwmctrl.h"

#define pwmctrlDEBUG	1

/* Dimensions the buffer into which input characters are placed. */
#define pwmctrlMAX_INPUT_SIZE		50
#define pwmctrlMAX_OUTPUT_SIZE		50

/* Place holder for calls to ioctl that don't use the value parameter. */
#define pwmctrlPARAMETER_NOT_USED	( ( void * ) 0 )

/* Block times of 50 and 500milliseconds, specified in ticks. */
#define pwmctrl50ms					( ( void * ) ( 50UL / portTICK_RATE_MS ) )
#define pwmctrl500ms				( ( void * ) ( 500UL / portTICK_RATE_MS ) )

#define pwmctrlSTOP					'\n'
#define pwmctrlDELIM				' '

static xTaskHandle xUARTReceiverTask = NULL, xPWMControllerTask = NULL;

static Peripheral_Descriptor_t xUART3 = NULL;

xQueueHandle xParameterQueue = 0, xCommandQueue = 0;

void vUARTStart( void ) {

	xCommandQueue = xQueueCreate(4, sizeof( int8_t ) );
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

	xUART3 = FreeRTOS_open( boardUART3, ( uint32_t ) pwmctrlPARAMETER_NOT_USED );
	configASSERT( xUART3 );

	xReturned = FreeRTOS_ioctl( xUART3, ioctlUSE_ZERO_COPY_TX, pwmctrlPARAMETER_NOT_USED );
	configASSERT( xReturned );

	xReturned = FreeRTOS_ioctl( xUART3, ioctlUSE_CHARACTER_QUEUE_RX, ( void * ) pwmctrlMAX_INPUT_SIZE );
	configASSERT( xReturned );

	xReturned = FreeRTOS_ioctl( xUART3, ioctlSET_INTERRUPT_PRIORITY, ( void * ) ( configMIN_LIBRARY_INTERRUPT_PRIORITY - 1 ) );
	configASSERT( xReturned );

	for(;;) {
		FreeRTOS_read( xUART3, &cRxedChar, sizeof( cRxedChar ) );

#if pwmctrlDEBUG
		if( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
			FreeRTOS_write( xUART3, &cRxedChar, sizeof( cRxedChar ) );
		}
#endif
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
	int8_t pcParameter[5];
	int8_t cCommand, cParameterIndex = 0, cPulseIndex = 0;
	int16_t sPulse[4];

	cCommand = *pcUartInput;

	pcUartInput++;

	while( pwmctrlDELIM == *pcUartInput && '\0' != *pcUartInput) {
		pcUartInput++;
	}

	while( '\0' != *pcUartInput ) {
		memset( pcParameter, '\0', 5);
		while( ( pwmctrlDELIM != *pcUartInput ) && ( '\0' != *pcUartInput ) ) {
			pcParameter[cParameterIndex] = *pcUartInput;
			cParameterIndex++;
			pcUartInput++;
		}
#if pwmctrlDEBUG
		if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
			FreeRTOS_write( xUART3, pcParameter, strlen( pcParameter ) );
		}
		if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
			FreeRTOS_write( xUART3, "\r\n", strlen( "\r\n" ) );
		}
#endif
		sPulse[cPulseIndex] = atoi( (const char * ) pcParameter );
		cParameterIndex = 0;
		pcUartInput++;
		cPulseIndex++;
	}
	xQueueSend( xParameterQueue, sPulse, portMAX_DELAY );
	xQueueSend( xCommandQueue, &cCommand, portMAX_DELAY );
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

	LPC_SC->PCONP |= ( 1 << 6 );

	LPC_PWM1->TCR = ( 1 << 1 );
	LPC_PWM1->IR = 0xff;

	LPC_PWM1->PR = SystemCoreClock / ( 4 * 1000000 ) - 1;

	LPC_PINCON->PINSEL4 &= ~( 0xf << 0 );
	LPC_PINCON->PINSEL4 |= ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 4 ) | ( 1 << 6 );

	LPC_PINCON->PINMODE4 &= ~( 0xff << 0 );
	LPC_PINCON->PINMODE4 |= ( 2 << 0 ) | ( 2 << 2 ) | ( 2 << 4 ) | ( 2 << 6 );

	LPC_PWM1->MR0 = 20000;
	LPC_PWM1->MR5 = 19000;

	LPC_PWM1->MR1 = 1500;
	LPC_PWM1->MR2 = 1500;
	LPC_PWM1->MR3 = 1500;
	LPC_PWM1->MR4 = 1500;

	LPC_PWM1->LER = 0x27;
	LPC_PWM1->MCR = ( 1 << 1 ) | ( 1 << 15 );
	LPC_PWM1->PCR = ( 1 << 9 ) | ( 1 << 10 ) | ( 1 << 11 ) | ( 1 << 12 );

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
		if ( 'a' == cCommand ) {
			LPC_PWM1->MR1 = sParameters[0];
			LPC_PWM1->MR2 = sParameters[1];
			LPC_PWM1->MR3 = sParameters[2];
			LPC_PWM1->MR4 = sParameters[3];
		} else if ('r' == cCommand ) {
			LPC_PWM1->MR1 += sParameters[0];
			LPC_PWM1->MR2 += sParameters[1];
			LPC_PWM1->MR3 += sParameters[2];
			LPC_PWM1->MR4 += sParameters[3];
		} else if ('p' == cCommand ) {
			switch(sParameters[0]) {
			case 1:	itoa( LPC_PWM1->MR1, cServoPosition, 16);
					break;
			case 2:	itoa( LPC_PWM1->MR2, cServoPosition, 16);
					break;
			case 3:	itoa( LPC_PWM1->MR3, cServoPosition, 16);
					break;
			case 4:	itoa( LPC_PWM1->MR4, cServoPosition, 16);
					break;
			}
			if ( FreeRTOS_ioctl( xUART3, ioctlOBTAIN_WRITE_MUTEX, pwmctrl50ms ) == pdPASS ) {
				FreeRTOS_write( xUART3, cServoPosition, strlen( cServoPosition ) );
			}
		}
		LPC_PWM1->LER = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 );
		xQueueReset( xCommandQueue );
		xQueueReset( xParameterQueue );
	}
}
