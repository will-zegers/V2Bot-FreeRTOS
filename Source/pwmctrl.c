#include <string.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IO.h"
#include "queue.h"

#include "pwmctrl.h"

#define pwmctrlDEBUG	0

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

xQueueHandle qh = 0;

void vUARTStart( void ) {

	qh = xQueueCreate(1, 4 * sizeof( int16_t ) );

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
	int8_t pcParameter[20];
	int8_t cCommand, cParameterIndex = 0, cPulseIndex = 0;
	uint16_t usPulse[4];

	cCommand = *pcUartInput;
	( void ) cCommand;

	pcUartInput++;

	while( pwmctrlDELIM == *pcUartInput && '\0' != *pcUartInput) {
		pcUartInput++;
	}

	while( '\0' != *pcUartInput ) {
		memset( pcParameter, '\0', 20);
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
		usPulse[cPulseIndex] = atoi( (const char * ) pcParameter );
		cParameterIndex = 0;
		pcUartInput++;
		cPulseIndex++;
	}
	xQueueSend( qh, usPulse, portMAX_DELAY );
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

	uint16_t usPulse[4];

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
		xQueueReceive( qh, usPulse, portMAX_DELAY );

		LPC_PWM1->MR1 = usPulse[0];
		LPC_PWM1->MR2 = usPulse[1];
		LPC_PWM1->MR3 = usPulse[2];
		LPC_PWM1->MR4 = usPulse[3];
		LPC_PWM1->LER = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 );
	}
}
