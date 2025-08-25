################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_RTT.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_RTT_printf.c \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_SYSVIEW.c 

S_UPPER_SRCS += \
C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.S 

OBJS += \
./segger-sysview/SEGGER/SEGGER_RTT.o \
./segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.o \
./segger-sysview/SEGGER/SEGGER_RTT_printf.o \
./segger-sysview/SEGGER/SEGGER_SYSVIEW.o 

S_UPPER_DEPS += \
./segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.d 

C_DEPS += \
./segger-sysview/SEGGER/SEGGER_RTT.d \
./segger-sysview/SEGGER/SEGGER_RTT_printf.d \
./segger-sysview/SEGGER/SEGGER_SYSVIEW.d 


# Each subdirectory must supply rules for building sources it contributes
segger-sysview/SEGGER/SEGGER_RTT.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_RTT.c segger-sysview/SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.S segger-sysview/SEGGER/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"
segger-sysview/SEGGER/SEGGER_RTT_printf.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_RTT_printf.c segger-sysview/SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
segger-sysview/SEGGER/SEGGER_SYSVIEW.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/SEGGER_SYSVIEW.c segger-sysview/SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-segger-2d-sysview-2f-SEGGER

clean-segger-2d-sysview-2f-SEGGER:
	-$(RM) ./segger-sysview/SEGGER/SEGGER_RTT.cyclo ./segger-sysview/SEGGER/SEGGER_RTT.d ./segger-sysview/SEGGER/SEGGER_RTT.o ./segger-sysview/SEGGER/SEGGER_RTT.su ./segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.d ./segger-sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.o ./segger-sysview/SEGGER/SEGGER_RTT_printf.cyclo ./segger-sysview/SEGGER/SEGGER_RTT_printf.d ./segger-sysview/SEGGER/SEGGER_RTT_printf.o ./segger-sysview/SEGGER/SEGGER_RTT_printf.su ./segger-sysview/SEGGER/SEGGER_SYSVIEW.cyclo ./segger-sysview/SEGGER/SEGGER_SYSVIEW.d ./segger-sysview/SEGGER/SEGGER_SYSVIEW.o ./segger-sysview/SEGGER/SEGGER_SYSVIEW.su

.PHONY: clean-segger-2d-sysview-2f-SEGGER

