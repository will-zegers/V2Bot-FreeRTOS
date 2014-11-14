################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_DriverInterface.c \
Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_i2c.c \
Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_ssp.c \
Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_uart.c 

OBJS += \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_DriverInterface.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_i2c.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_ssp.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_uart.o 

C_DEPS += \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_DriverInterface.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_i2c.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_ssp.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_DriverInterface.o: Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_DriverInterface.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Device\LPC17xx\SupportedBoards" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-CLI" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\portable\GCC\ARM_CM3" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\CMSISv2p00_LPC17xx\inc" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source\Examples\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source" -I"Z:\LPCXpresso\FreeRTOS+IO WS\lpc17xx.cmsis.driver.library\Include" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_i2c.o: Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Device\LPC17xx\SupportedBoards" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-CLI" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\portable\GCC\ARM_CM3" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\CMSISv2p00_LPC17xx\inc" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source\Examples\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source" -I"Z:\LPCXpresso\FreeRTOS+IO WS\lpc17xx.cmsis.driver.library\Include" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_ssp.o: Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_ssp.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Device\LPC17xx\SupportedBoards" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-CLI" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\portable\GCC\ARM_CM3" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\CMSISv2p00_LPC17xx\inc" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source\Examples\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source" -I"Z:\LPCXpresso\FreeRTOS+IO WS\lpc17xx.cmsis.driver.library\Include" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_uart.o: Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/FreeRTOS_lpc17xx_uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Device\LPC17xx\SupportedBoards" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-CLI" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\portable\GCC\ARM_CM3" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\CMSISv2p00_LPC17xx\inc" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source\Examples\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-V2Bot\Source" -I"Z:\LPCXpresso\FreeRTOS+IO WS\lpc17xx.cmsis.driver.library\Include" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


