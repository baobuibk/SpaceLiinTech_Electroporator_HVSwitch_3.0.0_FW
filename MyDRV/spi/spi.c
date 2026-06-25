/*
 * spi.c
 *
 *  Created on: Jun 24, 2026
 *      Author:
 */

 
#include "spi.h"

#include "stm32f4xx_ll_spi.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

void SPI_Init(spi_driver_t* p_spi,
		 	  SPI_TypeDef* _handle, IRQn_Type _irqn,
              uint8_t* _p_TX_buffer, uint32_t _TX_size,
              uint8_t* _p_RX_buffer, uint32_t _RX_size,
              GPIO_TypeDef* _cs_port, uint32_t _cs_pin)
{
    p_spi->handle  = _handle;
    p_spi->irqn    = _irqn;

    p_spi->p_TX_buffer = _p_TX_buffer;
    p_spi->TX_size = _TX_size;
    p_spi->TX_write_index = 0;
    p_spi->TX_read_index  = 0;

    p_spi->p_RX_buffer = _p_RX_buffer;
    p_spi->RX_size = _RX_size;
    p_spi->RX_write_index = 0;
    p_spi->RX_read_index  = 0;

    p_spi->cs_port = _cs_port;
    p_spi->cs_pin  = _cs_pin;

    LL_GPIO_SetOutputPin(p_spi->cs_port, p_spi->cs_pin);

    LL_SPI_DisableIT_TXE(p_spi->handle);
    LL_SPI_DisableIT_RXNE(p_spi->handle);

    LL_SPI_Enable(p_spi->handle);
}


bool SPI_is_buffer_full(volatile uint32_t *pRead, volatile uint32_t *pWrite, uint32_t Size) {
    return (((( *pWrite + 1) % Size) == *pRead) ? 1 : 0);
}

bool SPI_is_buffer_empty(volatile uint32_t *pRead, volatile uint32_t *pWrite) {
    return (*pRead == *pWrite) ? 1 : 0;
}

uint32_t SPI_advance_buffer_index(volatile uint32_t* pIndex, uint32_t Size) {
    *pIndex = (*pIndex + 1) % Size;
    return *pIndex;
}


bool SPI_Transmit_IT(spi_driver_t* p_spi, const uint8_t* pData, uint16_t Size)
{
    for (uint16_t i = 0; i < Size; i++) {
        if (SPI_is_buffer_full(&p_spi->TX_read_index, &p_spi->TX_write_index, p_spi->TX_size)) {
            return false;
        }
        p_spi->p_TX_buffer[p_spi->TX_write_index] = pData[i];
        SPI_advance_buffer_index(&p_spi->TX_write_index, p_spi->TX_size);
    }

    LL_GPIO_ResetOutputPin(p_spi->cs_port, p_spi->cs_pin);

    LL_SPI_EnableIT_TXE(p_spi->handle);
    LL_SPI_EnableIT_RXNE(p_spi->handle);

    return true;
}



bool SPI_Receive_IT(spi_driver_t* p_spi, uint16_t Size)
{
    for (uint16_t i = 0; i < Size; i++) {
        if (SPI_is_buffer_full(&p_spi->TX_read_index, &p_spi->TX_write_index, p_spi->TX_size)) {
            return false;
        }
        p_spi->p_TX_buffer[p_spi->TX_write_index] = 0xFF;
        SPI_advance_buffer_index(&p_spi->TX_write_index, p_spi->TX_size);
    }

    LL_GPIO_ResetOutputPin(p_spi->cs_port, p_spi->cs_pin);

    LL_SPI_EnableIT_TXE(p_spi->handle);
    LL_SPI_EnableIT_RXNE(p_spi->handle);

    return true;
}

// && LL_SPI_IsEnabledIT_RXNE(p_spi->handle)
//&& LL_SPI_IsEnabledIT_TXE(p_spi->handle)

void SPI_IRQHandler_Callback(spi_driver_t* p_spi)
{

    if (LL_SPI_IsActiveFlag_RXNE(p_spi->handle))
    {
        uint8_t rx_data = LL_SPI_ReceiveData8(p_spi->handle);

        if (!SPI_is_buffer_full(&p_spi->RX_read_index, &p_spi->RX_write_index, p_spi->RX_size)) {
            p_spi->p_RX_buffer[p_spi->RX_write_index] = rx_data;
            SPI_advance_buffer_index(&p_spi->RX_write_index, p_spi->RX_size);
        }
    }


    if (LL_SPI_IsActiveFlag_TXE(p_spi->handle))
    {
        if (!SPI_is_buffer_empty(&p_spi->TX_read_index, &p_spi->TX_write_index))
        {
            LL_SPI_TransmitData8(p_spi->handle, p_spi->p_TX_buffer[p_spi->TX_read_index]);
            SPI_advance_buffer_index(&p_spi->TX_read_index, p_spi->TX_size);
        }
        else
        {
            LL_SPI_DisableIT_TXE(p_spi->handle);

            while (LL_SPI_IsActiveFlag_BSY(p_spi->handle));

            LL_GPIO_SetOutputPin(p_spi->cs_port, p_spi->cs_pin);

            LL_SPI_DisableIT_RXNE(p_spi->handle);
        }
    }
}

