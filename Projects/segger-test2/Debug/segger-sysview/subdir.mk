################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER_SYSVIEW_Config_NoOS.c 

OBJS += \
./segger-sysview/SEGGER_SYSVIEW_Config_NoOS.o 

C_DEPS += \
./segger-sysview/SEGGER_SYSVIEW_Config_NoOS.d 


# Each subdirectory must supply rules for building sources it contributes
segger-sysview/SEGGER_SYSVIEW_Config_NoOS.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER_SYSVIEW_Config_NoOS.c segger-sysview/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-segger-2d-sysview

clean-segger-2d-sysview:
	-$(RM) ./segger-sysview/SEGGER_SYSVIEW_Config_NoOS.cyclo ./segger-sysview/SEGGER_SYSVIEW_Config_NoOS.d ./segger-sysview/SEGGER_SYSVIEW_Config_NoOS.o ./segger-sysview/SEGGER_SYSVIEW_Config_NoOS.su

.PHONY: clean-segger-2d-sysview

