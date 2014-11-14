/*
 * pwmctrl.h
 *
 *  Created on: Nov 11, 2014
 *      Author: Will
 */

#ifndef UART_H_
#define UART_H_

void prvUARTReceiver(void * pvParameters);

void vUARTStart( void );

void prvParseCommand( const int8_t * pcUartInput );

void vPWMControllerStart( void );

void prvPWMController( void *pvrParamters );

#endif /* UART_H_ */
