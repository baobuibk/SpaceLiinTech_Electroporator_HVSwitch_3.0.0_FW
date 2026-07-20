        /*
 * vom_log.c
 *
 *  Created on: Jul 10, 2026
 *      Author: PV
 */

#include "vom_log.h"


spi_driver_t vom_is66w_spi = {
		.handle = IS66W_SPI_HANDLE,
		.dma_handle = IS66W_SPI_DMA_HANDLE,
		.dma_channel = IS66W_SPI_DMA_CHANNEL,
		.dma_tx_stream = IS66W_SPI_DMA_TX_STREAM,
		.dma_rx_stream = IS66W_SPI_DMA_RX_STREAM,
		.transmit_done = false,
};
is66_psram_t vom_is66w_dev = {
		.spi_drv = &vom_is66w_spi,
		.cs_port = IS66W_SPI_CS_PORT,
		.cs_pin = IS66W_SPI_CS_PIN,
		.IS66_id = {0}
};

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

vom_data_block_t VOM_DATA_BLOCK_A = {
	.sample_count = 0,
	.block_index = 0,
	.start_timestamp_us = 0,
	.samples = {{0}}
};

vom_data_block_t VOM_DATA_BLOCK_B = {
	.sample_count = 0,
	.block_index = 0,
	.start_timestamp_us = 0,
	.samples = {{0}}
};

vom_log_handle_t vom_log_handle = {
	.psram_address = 0x000000,
	.p_vom_data_block = &VOM_DATA_BLOCK_A
};

vom_data_t vom_data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false};

/* ---------------------------- STATIC VARIABLES ------------------------------------------*/

static vom_data_block_t VOM_DEBUG_BLOCK = {
    .sample_count = 0,
    .block_index = 0,
    .start_timestamp_us = 0,
    .samples = {{0}}
};

/* ---------------------------- INIT FUNCTION ------------------------------------------*/

void VOM_Psram_Init(void){
	IS66_Init(&vom_is66w_dev);
	IS66_ReadID(&vom_is66w_dev, vom_is66w_dev.IS66_id);
}

void VOM_INA229_Init(void){
    INA229_Init(&vom_ina229_dev);

    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_CONFIG, 0x8000);
    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_ADC_CONFIG, 0xB000);
    INA229_WriteReg16(&vom_ina229_dev, INA229_REG_DIAG_ALRT, 0x8001);

    INA229_Calibrate(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, MAX_EXPECTED_AMPS);

    INA229_Set_ShuntOverVoltage(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, 10.0);
    INA229_Set_ShuntUnderVoltage(&vom_ina229_dev, SHUNT_RESISTOR_OHMS, -10.0);

    INA229_Set_BusOverVoltage(&vom_ina229_dev, 310);
    INA229_Set_BusUnderVoltage(&vom_ina229_dev, 0);
}

void VOM_Timbase_Sampling_Init(void){

    LL_TIM_SetPrescaler(VOM_TIM_HANDLE, 71);
    LL_TIM_SetAutoReload(VOM_TIM_HANDLE, 49);
    LL_TIM_SetCounterMode(VOM_TIM_HANDLE, LL_TIM_COUNTERMODE_UP);

    LL_TIM_DisableCounter(VOM_TIM_HANDLE);
    LL_TIM_DisableIT_UPDATE(VOM_TIM_HANDLE);
}

void VOM_Log_Read_Block_Debug(uint16_t block_index) {
	    uint32_t target_address = (uint32_t)block_index * sizeof(vom_data_block_t);

	    char uart_buf[128];

	    if (target_address >= 0x800000) {
	        UART_Driver_SendString(&XBEE_UART, "\r\n[ERROR] PSRAM Address out of bounds!\r\n");
	        return;
	    }

	    IS66_Read(&vom_is66w_dev, target_address, (uint8_t*)&VOM_DEBUG_BLOCK, sizeof(vom_data_block_t));

	    snprintf(uart_buf, sizeof(uart_buf), "\r\n========== VOM PSRAM BLOCK [%u] ==========\r\n", VOM_DEBUG_BLOCK.block_index);
	    UART_Driver_SendString(&XBEE_UART, uart_buf);

	    snprintf(uart_buf, sizeof(uart_buf), "Start Timestamp : %lu us\r\n", VOM_DEBUG_BLOCK.start_timestamp_us);
	    UART_Driver_SendString(&XBEE_UART, uart_buf);

	    snprintf(uart_buf, sizeof(uart_buf), "Sample Count    : %u\r\n", VOM_DEBUG_BLOCK.sample_count);
	    UART_Driver_SendString(&XBEE_UART, uart_buf);

	    UART_Driver_SendString(&XBEE_UART, "------------------------------------------\r\n");
	    UART_Driver_SendString(&XBEE_UART, "Idx\tVoltage (mV)\tCurrent (mA)\r\n");
	    UART_Driver_SendString(&XBEE_UART, "------------------------------------------\r\n");

	    uint16_t print_count = VOM_DEBUG_BLOCK.sample_count;
	    if (print_count > VOM_LOG_SAMPLE_SIZE_MAX) {
	        print_count = VOM_LOG_SAMPLE_SIZE_MAX;
	    }

	    for (uint16_t i = 0; i < print_count; i++) {
	            uint32_t voltage_mV = (uint32_t)VOM_DEBUG_BLOCK.samples[i].voltage * 10;

	            uint32_t current_mA = (uint32_t)VOM_DEBUG_BLOCK.samples[i].current;

	            snprintf(uart_buf, sizeof(uart_buf), "%u\t%lu\t\t%lu\r\n", i, voltage_mV, current_mA);
	            UART_Driver_SendString(&XBEE_UART, uart_buf);
	        }

	    UART_Driver_SendString(&XBEE_UART, "================ END BLOCK ===============\r\n");
}

void VOM_Log_Get_HexDump(uint32_t start_address, uint32_t length) {
    char uart_buf[128];
    uint8_t read_buf[16];
    uint32_t current_addr = start_address;
    uint32_t remaining = length;

    if (start_address + length > 0x800000) {
        UART_Driver_SendString(&XBEE_UART, "\r\n[ERROR] HexDump: Address out of bounds!\r\n");
        return;
    }

    snprintf(uart_buf, sizeof(uart_buf), "\r\n------- PSRAM DATA (Addr: 0x%06lX, Length: %lu) -------\r\n", start_address, length);
    UART_Driver_SendString(&XBEE_UART, uart_buf);

    while (remaining > 0) {
        uint32_t chunk_size = (remaining > 16) ? 16 : remaining;
        IS66_Read(&vom_is66w_dev, current_addr, read_buf, chunk_size);
        int offset = snprintf(uart_buf, sizeof(uart_buf), "0x%06lX: ", current_addr);
        for (uint32_t i = 0; i < chunk_size; i++) {
            if (offset < sizeof(uart_buf)) {
                offset += snprintf(uart_buf + offset, sizeof(uart_buf) - offset, "%02X ", read_buf[i]);
            }
        }
        if (offset < sizeof(uart_buf)) {
            snprintf(uart_buf + offset, sizeof(uart_buf) - offset, "\r\n");
        }
        UART_Driver_SendString(&XBEE_UART, uart_buf);
        current_addr += chunk_size;
        remaining -= chunk_size;
    }
    UART_Driver_SendString(&XBEE_UART, "--------------------------------------------------------\r\n");
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


void VOM_Sampling_SetFrequency(uint32_t sampling_freq_hz){
    if (sampling_freq_hz == 0) return;
    uint32_t psc = 0;
    uint32_t arr = 0;

    if (sampling_freq_hz >= 16){
        psc = 71;
        arr = (1000000 / sampling_freq_hz) - 1;
    }
    else{
        psc = 71999;
        arr = (1000 / sampling_freq_hz) - 1;
    }
    LL_TIM_SetPrescaler(VOM_TIM_HANDLE, psc);
    LL_TIM_SetAutoReload(VOM_TIM_HANDLE, arr);
    LL_TIM_SetCounter(VOM_TIM_HANDLE, 0);
}





