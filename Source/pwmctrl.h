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

/**
 * prvParseCommand: Upon receiving the newline character, the UART receiver passes the command received to
 * prvParseCommand. This function takes byte 1 for the command (e.g., 'a' for abs, 'r' for rel), the next three
 * bytes for respective servo positions - which can be either negative or positive - a single byte to control the
 * platform via GPIO, and then computes a simple XOR checksum to check for errors in the communications.
 *
 * Parameters: pcUartInput: a point to the command string passed from the UART receiver.
 */
void prvParseCommand( const int8_t * pcUartInput );

/* Creates the task for the PWM controller. */
void vArmControllerTaskStart( void );

/* Waits for the command and parameter queues, the sends PWM signals based
 * on the command and parameters. At the moment, the implemented commands are:
 * [a]bsolute: sets all four servos to an absolute position.
 * [r]elative: spins the servos cw (positive pulse) or ccw (negative pulse) relative
 * 			   to its current position.
 * [p]osition: transmits, via UART3, the a character array with the position of the
 * 			   servo in decimal.
 */

/**
 * prvInitPWM(): initializes the PWM pins on the LPC1769
 */
void prvInitPWM();

/* prvArmController: receives the positional command for the arm assembly, and uses parameters passed
 * through the PWM queue to put the servos into their new positions, moving the arm.
 */
void prvArmController( void *pvrParamters );

/* Starts the controller for the platform */
void vBaseControllerTaskStart( void );

/** prvBaseController: receives the movement (i.e., forward, reverse, left, right, stop) for the moving
 * base platform form the PlatformGPIO Queue, and sets or clear the GPIO pins to drive this movement.
 */
void prvBaseController( void *prvParameters );

#endif /* UART_H_ */
