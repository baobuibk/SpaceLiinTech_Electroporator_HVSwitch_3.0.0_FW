/*
 * spi.h
 *
 *  Created on: Jun 24, 2026
 *      Author: PV
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_


#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_spi.h"

#include "stdbool.h"

typedef struct _spi_stdio_typedef
{
    SPI_TypeDef* 		handle;
    IRQn_Type         irqn;

    uint8_t* p_TX_buffer;
    volatile uint32_t TX_write_index;
    volatile uint32_t TX_read_index;
    uint32_t          TX_size;

    uint8_t* p_RX_buffer;
    volatile uint32_t RX_write_index;
    volatile uint32_t RX_read_index;
    uint32_t          RX_size;

    GPIO_TypeDef* cs_port;
    uint32_t          cs_pin;

} spi_driver_t;


void SPI_Init	(spi_driver_t* p_spi,
				SPI_TypeDef* _handle, IRQn_Type _irqn,
				uint8_t* _p_TX_buffer, uint32_t _TX_size,
				uint8_t* _p_RX_buffer, uint32_t _RX_size,
				GPIO_TypeDef* _cs_port, uint32_t _cs_pin);


bool SPI_Transmit_IT(spi_driver_t* p_spi, const uint8_t* pData, uint16_t Size);
bool SPI_Receive_IT(spi_driver_t* p_spi, uint16_t Size);


bool  SPI_is_buffer_full(volatile uint32_t *pRead, volatile uint32_t *pWrite, uint32_t Size);
bool  SPI_is_buffer_empty(volatile uint32_t *pRead, volatile uint32_t *pWrite);
uint32_t  SPI_advance_buffer_index(volatile uint32_t* pIndex, uint32_t Size);


void SPI_IRQHandler_Callback(spi_driver_t* p_spi);

#endif /* SPI_SPI_H_ */
