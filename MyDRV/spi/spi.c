/*
 * spi.c
 *
 *  Created on: Jun 24, 2026
 *      Author:
 */

 
#include "spi.h"

#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_dma.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>



static void SPI_Clear_DMA_Flags_Internal(DMA_TypeDef* DMAx, uint32_t Stream);



void SPI_Init(spi_driver_t* p_spi)
{

    LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_tx_stream);
	LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_rx_stream);

	LL_DMA_SetMode(p_spi->dma_handle, p_spi->dma_tx_stream, LL_DMA_MODE_NORMAL);
	LL_DMA_SetMode(p_spi->dma_handle, p_spi->dma_rx_stream, LL_DMA_MODE_NORMAL);

	LL_DMA_SetPeriphAddress(p_spi->dma_handle, p_spi->dma_tx_stream, (uint32_t)&(p_spi->handle->DR));
	LL_DMA_SetPeriphAddress(p_spi->dma_handle, p_spi->dma_rx_stream, (uint32_t)&(p_spi->handle->DR));

    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_tx_stream);
    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_rx_stream);

    LL_DMA_EnableIT_TC(p_spi->dma_handle, p_spi->dma_rx_stream);

    LL_SPI_Enable(p_spi->handle);
}


bool SPI_Transmit_Polling(spi_driver_t* p_spi, const uint8_t* pData, uint16_t Size)
{
    for (uint16_t i = 0; i < Size; i++)
    {
        while (!LL_SPI_IsActiveFlag_TXE(p_spi->handle));
        LL_SPI_TransmitData8(p_spi->handle, pData[i]);
        
        // Đọc dữ liệu ảo để xóa cờ RXNE, tránh lỗi Overrun (OVR)
        while (!LL_SPI_IsActiveFlag_RXNE(p_spi->handle));
        LL_SPI_ReceiveData8(p_spi->handle); 
    }
    while (LL_SPI_IsActiveFlag_BSY(p_spi->handle));
    return true;
}

bool SPI_Receive_Polling(spi_driver_t* p_spi, uint8_t* pData, uint16_t Size)
{
    for (uint16_t i = 0; i < Size; i++)
    {
        while (!LL_SPI_IsActiveFlag_TXE(p_spi->handle));
        LL_SPI_TransmitData8(p_spi->handle, 0xFF); 
        
        while (!LL_SPI_IsActiveFlag_RXNE(p_spi->handle));
        pData[i] = LL_SPI_ReceiveData8(p_spi->handle);
    }
    while (LL_SPI_IsActiveFlag_BSY(p_spi->handle));
    return true;
}

bool SPI_TransmitReceive_Polling(spi_driver_t* p_spi, const uint8_t* pTxData, uint8_t* pRxData, uint16_t Size)
{
    for (uint16_t i = 0; i < Size; i++)
    {
        while (!LL_SPI_IsActiveFlag_TXE(p_spi->handle));
        LL_SPI_TransmitData8(p_spi->handle, pTxData[i]);
        
        while (!LL_SPI_IsActiveFlag_RXNE(p_spi->handle));
        pRxData[i] = LL_SPI_ReceiveData8(p_spi->handle);
    }
    while (LL_SPI_IsActiveFlag_BSY(p_spi->handle));
    return true;
}



bool SPI_Transmit_DMA(spi_driver_t* p_spi, const uint8_t* pData, uint16_t Size)
{
    if (p_spi->dma_handle == NULL) return false;

    LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_tx_stream);

    LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_tx_stream, (uint32_t)pData);
    LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_tx_stream, Size);
    
    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_tx_stream);

    LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_tx_stream);

    LL_SPI_EnableDMAReq_TX(p_spi->handle);

    return true;
}

bool SPI_Receive_DMA(spi_driver_t* p_spi, uint8_t* pData, uint16_t Size, const uint8_t* pDummyByte)
{
    if (p_spi->dma_handle == NULL || pDummyByte == NULL) return false;

    // 1. Cấu hình luồng Nhận (RX)
    LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_rx_stream);
    LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_rx_stream, (uint32_t)pData);
    LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_rx_stream, Size);

    // 2. Cấu hình luồng Truyền ảo (TX Dummy) để kích hoạt xung nhịp nhịp Clock cho SPI
    LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_tx_stream);
    LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_tx_stream, (uint32_t)pDummyByte);
    LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_tx_stream, Size);

    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_tx_stream);
    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_rx_stream);

    // Kích hoạt Request DMA từ phía SPI
    LL_SPI_EnableDMAReq_RX(p_spi->handle);
    LL_SPI_EnableDMAReq_TX(p_spi->handle);

    LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_rx_stream);
    LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_tx_stream);

    return true;
}

bool SPI_TransmitReceive_DMA(spi_driver_t* p_spi, const uint8_t* pTxData, uint8_t* pRxData, uint16_t Size)
{
    if (p_spi->dma_handle == NULL) return false;

    // RX
    LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_rx_stream);
    LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_rx_stream, (uint32_t)pRxData);
    LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_rx_stream, Size);

    // TX
    LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_tx_stream);
    LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_tx_stream, (uint32_t)pTxData);
    LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_tx_stream, Size);

    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_tx_stream);
    SPI_Clear_DMA_Flags_Internal(p_spi->dma_handle, p_spi->dma_rx_stream);

    LL_SPI_EnableDMAReq_RX(p_spi->handle);
    LL_SPI_EnableDMAReq_TX(p_spi->handle);

    LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_rx_stream);
    LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_tx_stream);


    return true;
}





/*--------------------------- STATIC FUNCTIONS -----------------------------------*/
static void SPI_Clear_DMA_Flags_Internal(DMA_TypeDef* DMAx, uint32_t Stream)
{
    switch (Stream)
    {
        case LL_DMA_STREAM_0:
            LL_DMA_ClearFlag_TC0(DMAx); LL_DMA_ClearFlag_HT0(DMAx); LL_DMA_ClearFlag_TE0(DMAx);
            break;
        case LL_DMA_STREAM_1:
            LL_DMA_ClearFlag_TC1(DMAx); LL_DMA_ClearFlag_HT1(DMAx); LL_DMA_ClearFlag_TE1(DMAx);
            break;
        case LL_DMA_STREAM_2:
            LL_DMA_ClearFlag_TC2(DMAx); LL_DMA_ClearFlag_HT2(DMAx); LL_DMA_ClearFlag_TE2(DMAx);
            break;
        case LL_DMA_STREAM_3:
            LL_DMA_ClearFlag_TC3(DMAx); LL_DMA_ClearFlag_HT3(DMAx); LL_DMA_ClearFlag_TE3(DMAx);
            break;
        case LL_DMA_STREAM_4:
            LL_DMA_ClearFlag_TC4(DMAx); LL_DMA_ClearFlag_HT4(DMAx); LL_DMA_ClearFlag_TE4(DMAx);
            break;
        case LL_DMA_STREAM_5:
            LL_DMA_ClearFlag_TC5(DMAx); LL_DMA_ClearFlag_HT5(DMAx); LL_DMA_ClearFlag_TE5(DMAx);
            break;
        case LL_DMA_STREAM_6:
            LL_DMA_ClearFlag_TC6(DMAx); LL_DMA_ClearFlag_HT6(DMAx); LL_DMA_ClearFlag_TE6(DMAx);
            break;
        case LL_DMA_STREAM_7:
            LL_DMA_ClearFlag_TC7(DMAx); LL_DMA_ClearFlag_HT7(DMAx); LL_DMA_ClearFlag_TE7(DMAx);
            break;
        default:
            break;
    }
}
