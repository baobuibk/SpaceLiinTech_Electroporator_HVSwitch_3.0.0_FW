################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyDRV/scheduler/scheduler.c 

OBJS += \
./MyDRV/scheduler/scheduler.o 

C_DEPS += \
./MyDRV/scheduler/scheduler.d 


# Each subdirectory must supply rules for building sources it contributes
MyDRV/scheduler/%.o MyDRV/scheduler/%.su MyDRV/scheduler/%.cyclo: ../MyDRV/scheduler/%.c MyDRV/scheduler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DSTM32F405xx -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/H_bridge_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/cmd_line_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/cli" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/fsp" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/pwm" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/scheduler" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/stm_header" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/uart" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/H_switch" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/V_switch" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MyDRV-2f-scheduler

clean-MyDRV-2f-scheduler:
	-$(RM) ./MyDRV/scheduler/scheduler.cyclo ./MyDRV/scheduler/scheduler.d ./MyDRV/scheduler/scheduler.o ./MyDRV/scheduler/scheduler.su

.PHONY: clean-MyDRV-2f-scheduler

