*******************************************************************************
FreeRTOS-Plus-V2BOT
by Tony Li, Hien Nguyen, Will Zegers, and Jackson Zhao
*******************************************************************************

Details about V2BOT control software and architecture can be 
found in "Implementation of the V2BOT Control Drivers on the LPC1769" PDF
included with this project.

To compile the project, import the directory "FreeRTOS-Plus-V2BOT" directly into
your Xpresso IDE workspace. You will also need to import the project
dependencies found in the "Dependent_Libraries.zip" file. You can import direct-
ly into the workspace from the ZIP file, and you should the following additional
projects in the workspace when you compile:

CMSISv2p00_LPC17xx
FreeRTOS-Plus-Demo-1
FreeRTOS-Products
lpc17xx.cmsis.driver.library

Once built, the program can either be run through debugging or flash directly
into the LPC1769 memory for execution.

The "CLI Control" directory includes Python scripts for simple, manual control
of V2BOT. Note that you will need the dependent libraries such as Pyserial to
run it.