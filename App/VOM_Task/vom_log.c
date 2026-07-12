/*
 * vom_log.c
 *
 *  Created on: Jul 10, 2026
 *      Author: PV
 */

#include "vom_log.h"

#include "board.h"

#include "stdbool.h"
#include "stdint.h"



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
	.samples = {0}
};

vom_data_block_t VOM_DATA_BLOCK_B = {
	.sample_count = 0,
	.block_index = 0,
	.start_timestamp_us = 0,
	.samples = {0}
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



