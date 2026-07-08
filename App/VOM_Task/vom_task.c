/*
 * vom_task.c
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#include "vom_task.h"
#include "spi.h"
#include "INA229.h"
#include "board.h"
#include "h_bridge_task.h"
#include "scheduler.h"
#include "xbee_cmd_task.h"

#include "stm32f4xx_ll_tim.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>



static float current_value [2000];
static float voltage_value [2000];
static uint16_t idx = 0;


spi_driver_t	vom_spi = {
		.handle = VOM_SPI_HANDLE,
		.dma_handle = VOM_SPI_DMA_HANDLE,
		.dma_channel = VOM_SPI_DMA_CHANNEL,
		.dma_tx_stream = VOM_SPI_DMA_TX_STREAM,
		.dma_rx_stream = VOM_SPI_DMA_RX_STREAM,
		.transmit_done = false,
};

ina229_t vom_ina229_dev = {
        .spi = &vom_spi,
        .cs_port = VOM_SPI_CS_PORT,
        .cs_pin = VOM_SPI_CS_PIN,
        .current_lsb = 0.0f,
};

vom_data_t vom_data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false};


static vom_sampling_state_t vom_sampling_state = VOM_SAMPLING_VOLTAGE;

/*----------------------------VOM FUNCTION----------------------------*/
void VOM_Init(void){
    INA229_Init(&vom_ina229_dev);

    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_CONFIG, 0x8000);
    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_ADC_CONFIG, 0xB000);
    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_DIAG_ALRT, 0x8001);

    INA229_Calibrate(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, MAX_EXPECTED_AMPS);

    INA229_Set_ShuntOverVoltage(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, 5.0);
    INA229_Set_ShuntUnderVoltage(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, -5.0);

    INA229_Set_BusOverVoltage(&vom_ina229_dev, 310);
    INA229_Set_BusUnderVoltage(&vom_ina229_dev, 0);
}

void VOM_Set_Current_Limit(float current_limit_amps){
    vom_data.current_limit = current_limit_amps;
    INA229_Set_ShuntOverVoltage(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, current_limit_amps);
}

float VOM_Get_Current_Limit(void){
	float cur_limit = INA229_Get_ShuntOverVoltage(&vom_ina229_dev);
    return (cur_limit/SHUNT_RESISTOR_OHMS);
}

bool VOM_Get_OVC_Flag(void){
    return vom_data.over_current_flag;
}

void VOM_Reset_OVC_Flag(void){
    INA229_Clear_AlertFlags(&vom_ina229_dev);
    vom_data.over_current_flag = false;
}

void VOM_Timbase_Sampling_Init(void){

    LL_TIM_SetPrescaler(VOM_TIM_HANDLE, 71);
    LL_TIM_SetAutoReload(VOM_TIM_HANDLE, 49);
    LL_TIM_SetCounterMode(VOM_TIM_HANDLE, LL_TIM_COUNTERMODE_UP);

    LL_TIM_DisableCounter(VOM_TIM_HANDLE);
    LL_TIM_DisableIT_UPDATE(VOM_TIM_HANDLE);
}

void VOM_Sampling_Start(void){

    LL_TIM_SetCounter(VOM_TIM_HANDLE, 0);

    LL_TIM_EnableIT_UPDATE(VOM_TIM_HANDLE);
    LL_TIM_EnableCounter(VOM_TIM_HANDLE);
}

void VOM_Sampling_Stop(void){

    LL_TIM_DisableCounter(VOM_TIM_HANDLE);
    LL_TIM_DisableIT_UPDATE(VOM_TIM_HANDLE);

    idx = 0;
//    memset(current_value, 0, sizeof(current_value));
//    memset(voltage_value, 0, sizeof(voltage_value));
}

void VOM_Sampling_SetFrequency(uint32_t sampling_freq_hz){

    if (sampling_freq_hz == 0) return;

    uint32_t psc = 0;
    uint32_t arr = 0;

    if (sampling_freq_hz >= 16)
    {
        psc = 71;
        arr = (1000000 / sampling_freq_hz) - 1;
    }
    else
    {
        psc = 71999;
        arr = (1000 / sampling_freq_hz) - 1;
    }

    LL_TIM_SetPrescaler(VOM_TIM_HANDLE, psc);
    LL_TIM_SetAutoReload(VOM_TIM_HANDLE, arr);

    LL_TIM_SetCounter(VOM_TIM_HANDLE, 0);
}


/*----------------------------VOM TASK FUNCTION----------------------------*/
void VOM_Task (void*){
//    vom_data.bus_voltage = INA229_GetBusVoltage(&vom_ina229_dev);
//    vom_data.shunt_voltage = INA229_GetShuntVoltage(&vom_ina229_dev);
//    vom_data.temperature = INA229_GetTemperature(&vom_ina229_dev);
//    vom_data.current = INA229_GetCurrent(&vom_ina229_dev);
//    vom_data.power = INA229_GetPower(&vom_ina229_dev);
//    vom_data.energy = INA229_GetEnergy(&vom_ina229_dev);
//    vom_data.charge = INA229_GetCharge(&vom_ina229_dev);
//	INA229_Start_ReadReg_DMA(&vom_ina229_dev, INA229_REG_VBUS, INA229_REG_VBUS_SIZE);
//	INA229_Start_ReadReg_DMA(&vom_ina229_dev, INA229_REG_CURRENT, INA229_REG_CURRENT_SIZE);

}


/*----------------------------VOM ISR FUCTION----------------------------*/
/*
* Timmer trigger ISR for VOM sampling every 50us
* Switch rotation between voltage and current sampling
* Sammling voltage first, then current, then voltage, then current...
* Sampling time total is 100us -> 10kHz sampling frequency
*/
void VOM_TIM_Sampling_Trigger_ISR(void){
    if (LL_TIM_IsActiveFlag_UPDATE(VOM_TIM_HANDLE))
    {
        LL_TIM_ClearFlag_UPDATE(VOM_TIM_HANDLE);
        switch (vom_sampling_state)
        {
            case VOM_SAMPLING_VOLTAGE:
                INA229_Start_ReadReg_DMA(&vom_ina229_dev, INA229_REG_VBUS, INA229_REG_VBUS_SIZE);
                break;

            case VOM_SAMPLING_CURRENT:
                INA229_Start_ReadReg_DMA(&vom_ina229_dev, INA229_REG_CURRENT, INA229_REG_CURRENT_SIZE);
                break;

            default:
                break;
        }
    }
}

void VOM_DMA_Receive_Complete_ISR(void) {
    if (LL_DMA_IsActiveFlag_TC0(VOM_SPI_DMA_HANDLE))
    {
        LL_DMA_ClearFlag_TC0(VOM_SPI_DMA_HANDLE);
        INA229_CS_High(&vom_ina229_dev);
        switch (vom_sampling_state)
        {
            case VOM_SAMPLING_VOLTAGE:
                voltage_value[idx] = INA229_Parse_BusVoltage_DMA(&vom_ina229_dev) * 33.333333;
                
                vom_sampling_state = VOM_SAMPLING_CURRENT;
                break;

            case VOM_SAMPLING_CURRENT:
                current_value[idx] = INA229_Parse_Current_DMA(&vom_ina229_dev);

                idx++;
                if (idx >= 2000) {
                    idx = 0; 
                }
                vom_sampling_state = VOM_SAMPLING_VOLTAGE;
                break;

            default:
                vom_sampling_state = VOM_SAMPLING_VOLTAGE;
                break;
        }
    }
}

void VOM_OVC_Handle_ISR(void) {
	H_Bridge_State = HB_TASK_ERROR_STATE;
    vom_data.over_current_flag = true;

    UART_Driver_SendString(&XBEE_UART, "OVER CURRENT DETECTED\r\n> ");

}

