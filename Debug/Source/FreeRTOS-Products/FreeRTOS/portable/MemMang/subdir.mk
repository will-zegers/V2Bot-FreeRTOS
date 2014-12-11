################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS/portable/MemMang/heap_2.c 

OBJS += \
./Source/FreeRTOS-Products/FreeRTOS/portable/MemMang/heap_2.o 

C_DEPS += \
./Source/FreeRTOS-Products/FreeRTOS/portable/MemMang/heap_2.d 


# Each subdirectory must supply rules for building sources it contributes
Source/FreeRTOS-Products/FreeRTOS/portable/MemMang/heap_2.o: Z:/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS/portable/MemMang/heap_2.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Device\LPC17xx\SupportedBoards" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-IO\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS-Plus-CLI" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\portable\GCC\ARM_CM3" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Products\FreeRTOS\include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\CMSISv2p00_LPC17xx\inc" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Plus-V2BOT\Source\Examples\Include" -I"Z:\LPCXpresso\FreeRTOS+IO WS\FreeRTOS-Plus-V2BOT\Source" -I"Z:\LPCXpresso\FreeRTOS+IO WS\lpc17xx.cmsis.driver.library\Include" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


