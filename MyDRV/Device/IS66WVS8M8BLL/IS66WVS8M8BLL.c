/*
 * IS66WVS8M8BLL.c
 *
 *  Created on: Jul 10, 2026
 *      Author: PV
 */

#include "IS66WVS8M8BLL.h"
#include "stm32f4xx_ll_spi.h"
#include "board.h"



void IS66_CS_Select(is66_psram_t* p_dev) {
    LL_GPIO_ResetOutputPin(p_dev->cs_port, p_dev->cs_pin);
}

void IS66_CS_Deselect(is66_psram_t* p_dev) {
    LL_GPIO_SetOutputPin(p_dev->cs_port, p_dev->cs_pin);
}

static bool IS66_Send_Cmd_Address(is66_psram_t* p_dev, uint8_t cmd, uint32_t address) 
{
    uint8_t header[4];
    header[0] = cmd;
    header[1] = (uint8_t)((address >> 16) & 0xFF); // Address 23:16
    header[2] = (uint8_t)((address >> 8) & 0xFF);  // Address 15:8
    header[3] = (uint8_t)(address & 0xFF);         // Address 7:0
    
    return SPI_Transmit_Polling(p_dev->spi_drv, header, 4);
}

bool IS66_Init(is66_psram_t* p_dev) 
{

    SPI_Init(p_dev->spi_drv);
    IS66_CS_Deselect(p_dev);
    
//   IS66_CS_Select(p_dev);
//   uint8_t cmd_reset_en = IS66_CMD_RESET_EN;
//   SPI_Transmit_Polling(p_dev->spi_drv, &cmd_reset_en, 1);
//   IS66_CS_Deselect(p_dev);
//
//   IS66_CS_Select(p_dev);
//   uint8_t cmd_reset = IS66_CMD_RESET;
//   SPI_Transmit_Polling(p_dev->spi_drv, &cmd_reset, 1);
//   IS66_CS_Deselect(p_dev);
//
    return true;
}

bool IS66_ReadID(is66_psram_t* p_dev, uint8_t* p_id_data) 
{
	if (p_dev == NULL || p_id_data == NULL) return false;

	uint8_t cmd_address[4];
	cmd_address[0] = IS66_CMD_READID;
	cmd_address[1] = 0x00;
	cmd_address[2] = 0x00;
	cmd_address[3] = 0x00;

	IS66_CS_Select(p_dev);
	SPI_Transmit_Polling(p_dev->spi_drv, cmd_address, 4);
	SPI_Receive_Polling(p_dev->spi_drv, p_id_data, 8);
	IS66_CS_Deselect(p_dev);
    
    return true;
}

bool IS66_Write(is66_psram_t* p_dev, uint32_t address, const uint8_t* p_data, uint16_t size) 
{
    IS66_CS_Select(p_dev);
    IS66_Send_Cmd_Address(p_dev, IS66_CMD_WRITE, address);
    SPI_Transmit_Polling(p_dev->spi_drv, p_data, size);
    IS66_CS_Deselect(p_dev);
    return true;
}

bool IS66_Read(is66_psram_t* p_dev, uint32_t address, uint8_t* p_data, uint16_t size) 
{
    IS66_CS_Select(p_dev);
    IS66_Send_Cmd_Address(p_dev, IS66_CMD_READ, address);
    SPI_Receive_Polling(p_dev->spi_drv, p_data, size);
    IS66_CS_Deselect(p_dev);
    return true;
}

bool IS66_FastRead(is66_psram_t* p_dev, uint32_t address, uint8_t* p_data, uint16_t size) 
{
    IS66_CS_Select(p_dev);
    IS66_Send_Cmd_Address(p_dev, IS66_CMD_FAST_READ, address);

    uint8_t dummy = 0xFF;
    SPI_Transmit_Polling(p_dev->spi_drv, &dummy, 1);
    SPI_Receive_Polling(p_dev->spi_drv, p_data, size);
    
    IS66_CS_Deselect(p_dev);
    return true;
}

/* --- NON-BLOCKING DMA FUNCTIONS --- */


bool IS66_Write_DMA(is66_psram_t* p_dev, uint32_t address, const uint8_t* p_data, uint16_t size) 
{
    IS66_CS_Select(p_dev);
    IS66_Send_Cmd_Address(p_dev, IS66_CMD_WRITE, address);
    
    // Bắt đầu truyền DMA phần Data
    return SPI_Transmit_DMA(p_dev->spi_drv, p_data, size);
}

bool IS66_FastRead_DMA(is66_psram_t* p_dev, uint32_t address, uint8_t* p_data, uint16_t size, const uint8_t* p_dummy_byte) 
{
    IS66_CS_Select(p_dev);
    IS66_Send_Cmd_Address(p_dev, IS66_CMD_FAST_READ, address);
    
    // Dummy clock
    SPI_Transmit_Polling(p_dev->spi_drv, p_dummy_byte, 1);
    
    // Bắt đầu nhận DMA phần Data
    return SPI_Receive_DMA(p_dev->spi_drv, p_data, size, p_dummy_byte);
}
