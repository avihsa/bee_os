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
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.o \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.o \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.o 

C_DEPS += \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.d \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.d \
./SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.d 


# Each subdirectory must supply rules for building sources it contributes
SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.c SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.c SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.o: C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.c SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/include" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/test/test_stm" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/Config" -I"C:/Users/kuris/Desktop/EZS-New/ezs_ss25/Projects/realtime_library/segger-sysview/SEGGER" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SEGGER-2f-Syscalls

clean-SEGGER-2f-Syscalls:
	-$(RM) ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.cyclo ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.su ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.cyclo ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.d ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.o ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_IAR.su ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.cyclo ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.d ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.o ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_KEIL.su ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.cyclo ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.d ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.o ./SEGGER/Syscalls/SEGGER_RTT_Syscalls_SES.su

.PHONY: clean-SEGGER-2f-Syscalls

