/*
 * pwmctrl.h
 *
 *  Created on: Nov 11, 2014
 *      Author: Will
 */

#ifndef UART_H_
#define UART_H_

/* Creates the two queue to pass the received command and parameter, and
 * then starts the UART Rx task.
 */
void prvUARTReceiver(void * pvParameters);

/* Starts UART3 for communications with either terminal (in debug mode) or
 * with another device using UART, such as the Friendly ARM board. Reads
 * character by character, and process the input once it is terminated. The
 * command is put onto the command queue, and the parameter array put into
 * the parameter queue to be read by the PWM controller.
 */
void vUARTStart( void );

/* Parses the command string received from UART. Reads the first character
 * as a command letter, followed by storing the rest of the delimited input
 * as parameters to the command.
 */
void prvParseCommand( const int8_t * pcUartInput );

/* Creates the task for the PWM controller. */
void vPWMControllerStart( void );

/* Waits for the command and parameter queues, the sends PWM signals based
 * on the command and parameters. At the moment, the implemented commands are:
 * [a]bsolute: sets all four servos to an absolute position.
 * [r]elative: spins the servos cw (positive pulse) or ccw (negative pulse) relative
 * 			   to its current position.
 * [p]osition: transmits, via UART3, the a character array with the position of the
 * 			   servo in decimal.
 */
void prvPWMController( void *pvrParamters );

#endif /* UART_H_ */
