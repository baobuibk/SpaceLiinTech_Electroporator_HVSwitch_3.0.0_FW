/*
 * spi.h
 *
 * Created on: Jun 24, 2026
 * Author: PV
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_dma.h"
#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"

#define SPI_TIMEOUT 500;

typedef enum {
	SPI_Error = 0,
	SPI_Success
}SPI_Status_t;

typedef struct _spi_stdio_typedef
{
    SPI_TypeDef*    handle;         

    DMA_TypeDef*    dma_handle;     
    uint32_t        dma_tx_stream;  
    uint32_t        dma_rx_stream;  
    uint32_t        dma_channel;

    bool            transmit_done;

} spi_driver_t;


void SPI_Init(spi_driver_t* p_spi);

// --- POLLING  ---
SPI_Status_t SPI_Transmit_Polling(spi_driver_t* p_spi, const uint8_t* pData, uint16_t Size);
SPI_Status_t SPI_Receive_Polling(spi_driver_t* p_spi, uint8_t* pData, uint16_t Size);
SPI_Status_t SPI_TransmitReceive_Polling(spi_driver_t* p_spi, const uint8_t* pTxData, uint8_t* pRxData, uint16_t Size);


// --- DMA (NON-BLOCKING) ---
bool SPI_Transmit_DMA(spi_driver_t* p_spi, const uint8_t* pData, uint16_t Size);
bool SPI_Receive_DMA(spi_driver_t* p_spi, uint8_t* pData, uint16_t Size, const uint8_t* pDummyByte);
bool SPI_TransmitReceive_DMA(spi_driver_t* p_spi, const uint8_t* pTxData, uint8_t* pRxData, uint16_t Size);

#endif /* SPI_SPI_H_ */
