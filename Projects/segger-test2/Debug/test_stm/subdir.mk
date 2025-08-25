################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm/test_tasks.c 

OBJS += \
./test_stm/test_tasks.o 

C_DEPS += \
./test_stm/test_tasks.d 


# Each subdirectory must supply rules for building sources it contributes
test_stm/test_tasks.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm/test_tasks.c test_stm/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-test_stm

clean-test_stm:
	-$(RM) ./test_stm/test_tasks.cyclo ./test_stm/test_tasks.d ./test_stm/test_tasks.o ./test_stm/test_tasks.su

.PHONY: clean-test_stm

