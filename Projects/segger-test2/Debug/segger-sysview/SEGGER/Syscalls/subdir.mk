################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.c 

OBJS += \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.o \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.o \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.o 

C_DEPS += \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.d \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.d \
./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.d 


# Each subdirectory must supply rules for building sources it contributes
segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c segger-sysview/SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.c segger-sysview/SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.c segger-sysview/SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.c segger-sysview/SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-segger-2d-sysview-2f-SEGGER-2f-Syscalls

clean-segger-2d-sysview-2f-SEGGER-2f-Syscalls:
	-$(RM) ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.cyclo ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.su ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.cyclo ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.d ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.o ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.su ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.cyclo ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.d ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.o ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.su ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.cyclo ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.d ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.o ./segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.su

.PHONY: clean-segger-2d-sysview-2f-SEGGER-2f-Syscalls

