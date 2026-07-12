################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyDRV/Device/INA229/INA229.c 

OBJS += \
./MyDRV/Device/INA229/INA229.o 

C_DEPS += \
./MyDRV/Device/INA229/INA229.d 


# Each subdirectory must supply rules for building sources it contributes
MyDRV/Device/INA229/%.o MyDRV/Device/INA229/%.su MyDRV/Device/INA229/%.cyclo: ../MyDRV/Device/INA229/%.c MyDRV/Device/INA229/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DSTM32F405xx -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/impedance_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/Device/IS66WVS8M8BLL" -I../Core/Inc -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/VOM_Task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/spi" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/Device" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/Device/INA229" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/fsp_line_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/H_bridge_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App/cmd_line_task" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/cli" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/fsp" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/pwm" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/scheduler" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/stm_header" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/uart" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/H_switch" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV/V_switch" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/App" -I"D:/LAB209B3/SpaceLiinTech_Eletroporator/DuAn_HV_V3_Firm/HVP_FIRM_V/MyDRV" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MyDRV-2f-Device-2f-INA229

clean-MyDRV-2f-Device-2f-INA229:
	-$(RM) ./MyDRV/Device/INA229/INA229.cyclo ./MyDRV/Device/INA229/INA229.d ./MyDRV/Device/INA229/INA229.o ./MyDRV/Device/INA229/INA229.su

.PHONY: clean-MyDRV-2f-Device-2f-INA229

