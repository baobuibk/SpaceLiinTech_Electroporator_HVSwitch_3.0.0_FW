/*
 * vom_task.c
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#include "vom_task.h"


static vom_sampling_state_t vom_sampling_state = VOM_SAMPLING_VOLTAGE;

/*----------------------------VOM FUNCTION----------------------------*/
void VOM_Init(void){
	VOM_INA229_Init();
	VOM_Psram_Init();
	VOM_Timbase_Sampling_Init();
}


void VOM_Sampling_Start(void){
    vom_log_handle.p_vom_data_block = &VOM_DATA_BLOCK_A;
    vom_log_handle.p_vom_data_block->block_index = 0;
    vom_log_handle.p_vom_data_block->sample_count = 0;

    VOM_DATA_BLOCK_B.block_index = 0;
    VOM_DATA_BLOCK_B.sample_count = 0;

    vom_log_handle.psram_address = 0x000000;

    vom_sampling_state = VOM_SAMPLING_VOLTAGE;

    LL_TIM_SetCounter(VOM_TIM_HANDLE, 0);
    LL_TIM_EnableIT_UPDATE(VOM_TIM_HANDLE);
    LL_TIM_EnableCounter(VOM_TIM_HANDLE);
}

void VOM_Sampling_Stop(void){
    LL_TIM_DisableCounter(VOM_TIM_HANDLE);
    LL_TIM_DisableIT_UPDATE(VOM_TIM_HANDLE);
    IS66_Write_DMA(&vom_is66w_dev, vom_log_handle.psram_address, (uint8_t*)vom_log_handle.p_vom_data_block, sizeof(vom_data_block_t));
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
* Save data to PSRAM after 2.54ms
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
    if (LL_DMA_IsActiveFlag_TC0(vom_ina229_dev.spi->dma_handle))
    {
        LL_DMA_ClearFlag_TC0(vom_ina229_dev.spi->dma_handle);
        INA229_CS_High(&vom_ina229_dev);
        
        switch (vom_sampling_state)
        {
            case VOM_SAMPLING_VOLTAGE:
                vom_log_handle.p_vom_data_block->samples[vom_log_handle.p_vom_data_block->sample_count].voltage =(uint16_t) (INA229_Parse_BusVoltage_DMA(&vom_ina229_dev) * 33.333333 * 100);
                
                vom_sampling_state = VOM_SAMPLING_CURRENT;
                break;

            case VOM_SAMPLING_CURRENT:

                vom_log_handle.p_vom_data_block->samples[vom_log_handle.p_vom_data_block->sample_count].current = (uint16_t) (INA229_Parse_Current_DMA(&vom_ina229_dev) * 1000.0f);
                
                vom_log_handle.p_vom_data_block->sample_count++;
                if (vom_log_handle.p_vom_data_block->sample_count >= VOM_LOG_SAMPLE_SIZE_MAX) 
                {
                    IS66_Write_DMA(&vom_is66w_dev, vom_log_handle.psram_address, (uint8_t*)vom_log_handle.p_vom_data_block, sizeof(vom_data_block_t));
                    uint16_t next_block_index = vom_log_handle.p_vom_data_block->block_index + 1;

                    if(vom_log_handle.p_vom_data_block == &VOM_DATA_BLOCK_A) vom_log_handle.p_vom_data_block = &VOM_DATA_BLOCK_B;
                    else vom_log_handle.p_vom_data_block = &VOM_DATA_BLOCK_A;

                    vom_log_handle.p_vom_data_block->sample_count = 0;
                    vom_log_handle.p_vom_data_block->block_index = next_block_index;
                }

                vom_sampling_state = VOM_SAMPLING_VOLTAGE;
                break;

            default:
                vom_sampling_state = VOM_SAMPLING_VOLTAGE;
                break;
        }
    }
}

void VOM_DMA_Trasmit_Complete_ISR(void) {
    if (LL_DMA_IsActiveFlag_TC3(vom_ina229_dev.spi->dma_handle))
    {
        LL_DMA_ClearFlag_TC3(vom_ina229_dev.spi->dma_handle);

    }
}
void VOM_Log_DMA_Transmit_Complete_ISR(void) {

    if (LL_DMA_IsActiveFlag_TC4(vom_is66w_dev.spi_drv->dma_handle))
    {
        LL_DMA_ClearFlag_TC4(vom_is66w_dev.spi_drv->dma_handle);

        while (LL_SPI_IsActiveFlag_BSY(vom_is66w_dev.spi_drv->dma_handle));

        IS66_CS_Deselect(&vom_is66w_dev);
        vom_log_handle.psram_address += sizeof(vom_data_block_t);

        if (vom_log_handle.psram_address >= IS66_MEM_SIZE_32) 
        {
            vom_log_handle.psram_address = 0x000000; 
        }
    }
}

void VOM_Log_DMA_Receive_Complete_ISR(void) {

    if (LL_DMA_IsActiveFlag_TC3(vom_is66w_dev.spi_drv->dma_handle))
    {
        LL_DMA_ClearFlag_TC3(vom_is66w_dev.spi_drv->dma_handle);
    }
}

void VOM_OVC_Handle_ISR(void) {
	H_Bridge_State = HB_TASK_ERROR_STATE;
    vom_data.over_current_flag = true;
    SchedulerTaskEnable(H_BRIDGE_TASK, 1);
    UART_Driver_SendString(&XBEE_UART, "OVER CURRENT DETECTED\r\n> ");
}

