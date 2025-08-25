################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/kernel.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/message_queue.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/mutex.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/semaphore.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/task.c 

OBJS += \
./src/kernel/kernel.o \
./src/kernel/message_queue.o \
./src/kernel/mutex.o \
./src/kernel/semaphore.o \
./src/kernel/task.o 

C_DEPS += \
./src/kernel/kernel.d \
./src/kernel/message_queue.d \
./src/kernel/mutex.d \
./src/kernel/semaphore.d \
./src/kernel/task.d 


# Each subdirectory must supply rules for building sources it contributes
src/kernel/kernel.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/kernel.c src/kernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/kernel/message_queue.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/message_queue.c src/kernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/kernel/mutex.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/mutex.c src/kernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/kernel/semaphore.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/semaphore.c src/kernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
src/kernel/task.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/src/kernel/task.c src/kernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-src-2f-kernel

clean-src-2f-kernel:
	-$(RM) ./src/kernel/kernel.cyclo ./src/kernel/kernel.d ./src/kernel/kernel.o ./src/kernel/kernel.su ./src/kernel/message_queue.cyclo ./src/kernel/message_queue.d ./src/kernel/message_queue.o ./src/kernel/message_queue.su ./src/kernel/mutex.cyclo ./src/kernel/mutex.d ./src/kernel/mutex.o ./src/kernel/mutex.su ./src/kernel/semaphore.cyclo ./src/kernel/semaphore.d ./src/kernel/semaphore.o ./src/kernel/semaphore.su ./src/kernel/task.cyclo ./src/kernel/task.d ./src/kernel/task.o ./src/kernel/task.su

.PHONY: clean-src-2f-kernel

