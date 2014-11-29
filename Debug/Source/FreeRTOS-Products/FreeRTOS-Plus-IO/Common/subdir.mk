################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_DriverInterface.c \
/media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_IOUtils.c \
/media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CharQueueTxAndRx.c \
/media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CircularBufferRx.c \
/media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_ZeroCopyTx.c 

OBJS += \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_DriverInterface.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_IOUtils.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CharQueueTxAndRx.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CircularBufferRx.o \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_ZeroCopyTx.o 

C_DEPS += \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_DriverInterface.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_IOUtils.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CharQueueTxAndRx.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CircularBufferRx.d \
./Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_ZeroCopyTx.d 


# Each subdirectory must supply rules for building sources it contributes
Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_DriverInterface.o: /media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_DriverInterface.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/SupportedBoards" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-CLI" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/portable/GCC/ARM_CM3" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/CMSISv2p00_LPC17xx/inc" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source/Examples/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/lpc17xx.cmsis.driver.library/Include" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_IOUtils.o: /media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/FreeRTOS_IOUtils.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/SupportedBoards" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-CLI" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/portable/GCC/ARM_CM3" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/CMSISv2p00_LPC17xx/inc" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source/Examples/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/lpc17xx.cmsis.driver.library/Include" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CharQueueTxAndRx.o: /media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CharQueueTxAndRx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/SupportedBoards" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-CLI" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/portable/GCC/ARM_CM3" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/CMSISv2p00_LPC17xx/inc" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source/Examples/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/lpc17xx.cmsis.driver.library/Include" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CircularBufferRx.o: /media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_CircularBufferRx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/SupportedBoards" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-CLI" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/portable/GCC/ARM_CM3" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/CMSISv2p00_LPC17xx/inc" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source/Examples/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/lpc17xx.cmsis.driver.library/Include" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_ZeroCopyTx.o: /media/will/Data/LPCXpresso/FreeRTOS+IO\ WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Common/IOUtils_ZeroCopyTx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_CMSIS=CMSISv2p00_LPC17xx -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Device/LPC17xx/SupportedBoards" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-IO/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS-Plus-CLI" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/portable/GCC/ARM_CM3" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-Products/FreeRTOS/include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/CMSISv2p00_LPC17xx/inc" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source/Examples/Include" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/FreeRTOS-V2Bot/Source" -I"/media/will/Data/LPCXpresso/FreeRTOS+IO WS/lpc17xx.cmsis.driver.library/Include" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wextra -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


