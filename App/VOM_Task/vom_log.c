/*
 * vom_log.c
 *
 *  Created on: Jul 10, 2026
 *      Author: PV
 */

#include "vom_log.h"
#include "xbee_cmd_task.h"

#include "board.h"

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"



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

vom_data_block_t VOM_DEBUG_BLOCK = {
    .sample_count = 0,
    .block_index = 0,
    .start_timestamp_us = 0,
    .samples = {{0}}
};

vom_log_handle_t vom_log_handle = {
	.psram_address = 0x000000,
	.p_vom_data_block = &VOM_DATA_BLOCK_A
};


void VOM_psram_init(void){
	IS66_Init(&vom_is66w_dev);
	IS66_ReadID(&vom_is66w_dev, vom_is66w_dev.IS66_id);

	vom_log_handle.p_vom_data_block = &VOM_DATA_BLOCK_A;
}


void VOM_Log_Stop(void){
	vom_log_handle.p_vom_data_block = &VOM_DATA_BLOCK_A;
	vom_log_handle.p_vom_data_block -> block_index = 0;
	vom_log_handle.psram_address = 0x000000;
}

void VOM_Log_Read_Block_Debug(uint16_t block_index) {
	// 1. Tính toán địa chỉ bắt đầu của block trên PSRAM
	    uint32_t target_address = (uint32_t)block_index * sizeof(vom_data_block_t);

	    // 2. Buffer đệm nhỏ để in từng dòng ra UART (tránh tràn Stack)
	    char uart_buf[128];

	    // 3. Kiểm tra ranh giới địa chỉ (giới hạn bộ nhớ 8MB là 0x7FFFFF)
	    if (target_address >= 0x800000) {
	        UART_Driver_SendString(&XBEE_UART, "\r\n[ERROR] PSRAM Address out of bounds!\r\n");
	        return;
	    }

	    // 4. Kéo nguyên block 1024 byte về SRAM nội
	    IS66_Read(&vom_is66w_dev, target_address, (uint8_t*)&VOM_DEBUG_BLOCK, sizeof(vom_data_block_t));

	    // 5. IN RA CONSOLE - PHẦN HEADER
	    snprintf(uart_buf, sizeof(uart_buf), "\r\n========== VOM PSRAM BLOCK [%u] ==========\r\n", VOM_DEBUG_BLOCK.block_index);
	    UART_Driver_SendString(&XBEE_UART, uart_buf);

	    snprintf(uart_buf, sizeof(uart_buf), "Start Timestamp : %lu us\r\n", VOM_DEBUG_BLOCK.start_timestamp_us);
	    UART_Driver_SendString(&XBEE_UART, uart_buf);

	    snprintf(uart_buf, sizeof(uart_buf), "Sample Count    : %u\r\n", VOM_DEBUG_BLOCK.sample_count);
	    UART_Driver_SendString(&XBEE_UART, uart_buf);

	    UART_Driver_SendString(&XBEE_UART, "------------------------------------------\r\n");
	    UART_Driver_SendString(&XBEE_UART, "Idx\tVoltage (mV)\tCurrent (mA)\r\n");
	    UART_Driver_SendString(&XBEE_UART, "------------------------------------------\r\n");

	    // 6. IN RA CONSOLE - PHẦN PAYLOAD (Dữ liệu lấy mẫu)
	    // Giới hạn an toàn, tránh vòng lặp chạy lố nếu file bị hỏng
	    uint16_t print_count = VOM_DEBUG_BLOCK.sample_count;
	    if (print_count > VOM_LOG_SAMPLE_SIZE_MAX) {
	        print_count = VOM_LOG_SAMPLE_SIZE_MAX;
	    }

	    for (uint16_t i = 0; i < print_count; i++) {
	            // Phục hồi giá trị ra số nguyên
	            // Điện áp: đang ở đơn vị 10mV -> nhân 10 để thành 1mV
	            // Ép kiểu (uint32_t) trước khi nhân để không bị tràn thanh ghi 16-bit
	            uint32_t voltage_mV = (uint32_t)VOM_DEBUG_BLOCK.samples[i].voltage * 10;

	            // Dòng điện: đang ở đơn vị 1mA -> giữ nguyên
	            uint32_t current_mA = (uint32_t)VOM_DEBUG_BLOCK.samples[i].current;

	            // Định dạng in số nguyên không dấu 32-bit (%lu)
	            snprintf(uart_buf, sizeof(uart_buf), "%u\t%lu\t\t%lu\r\n", i, voltage_mV, current_mA);
	            UART_Driver_SendString(&XBEE_UART, uart_buf);
	        }

	    UART_Driver_SendString(&XBEE_UART, "================ END BLOCK ===============\r\n");
}


/*----------------------------VOM LOG ISR FUCTION----------------------------*/

void VOM_Log_DMA_Transmit_Complete_ISR(void) {

    if (LL_DMA_IsActiveFlag_TC4(vom_is66w_spi.dma_handle)) 
    {
        LL_DMA_ClearFlag_TC4(vom_is66w_spi.dma_handle);

        while (LL_SPI_IsActiveFlag_BSY(vom_is66w_spi.handle));

        IS66_CS_Deselect(&vom_is66w_dev);
        vom_log_handle.psram_address += sizeof(vom_data_block_t);

        if (vom_log_handle.psram_address >= IS66_MEM_SIZE_32) 
        {
            vom_log_handle.psram_address = 0x000000; 
        }
    }
}

void VOM_Log_DMA_Receive_Complete_ISR(void) {

    if (LL_DMA_IsActiveFlag_TC3(vom_is66w_spi.dma_handle)) 
    {
        LL_DMA_ClearFlag_TC3(vom_is66w_spi.dma_handle);
    }
}



