/*
 * IS66WVS8M8BLL.h
 *
 *  Created on: Jul 10, 2026
 *      Author: PV
 */

#ifndef DEVICE_IS66WVS8M8BLL_IS66WVS8M8BLL_H_
#define DEVICE_IS66WVS8M8BLL_IS66WVS8M8BLL_H_

#include "spi.h"
#include "stm32f4xx_ll_gpio.h"
#include <stdint.h>
#include <stdbool.h>

#include "spi.h"
#include "stm32f4xx_ll_gpio.h"

#define IS66_MEM_SIZE_64        0x800000
#define IS66_MEM_SIZE_32        0x400000

#define IS66_CMD_READ           0x03    // Normal Read (Max 33MHz)
#define IS66_CMD_FAST_READ      0x0B    // Fast Read (Max 104MHz)
#define IS66_CMD_WRITE          0x02    // Write
#define IS66_CMD_RESET_EN       0x66    // Reset Enable
#define IS66_CMD_RESET          0x99    // Reset
#define IS66_CMD_READID         0x9F    // Read ID






typedef struct {
    spi_driver_t*   spi_drv;    // Con trỏ tới SPI Driver quản lý bus này
    GPIO_TypeDef*   cs_port;    // Cổng GPIO của chân CE# (Chip Select)
    uint32_t        cs_pin;     // Chân GPIO của chân CE#

    uint8_t         IS66_id[8];

} is66_psram_t;


bool IS66_Init(is66_psram_t* p_dev);

// Các hàm Polling (Blocking)

bool IS66_ReadID(is66_psram_t* p_dev, uint8_t* p_id_data);

bool IS66_Write(is66_psram_t* p_dev, uint32_t address, const uint8_t* p_data, uint16_t size);
bool IS66_Read(is66_psram_t* p_dev, uint32_t address, uint8_t* p_data, uint16_t size);
bool IS66_FastRead(is66_psram_t* p_dev, uint32_t address, uint8_t* p_data, uint16_t size);

// Các hàm DMA (Non-Blocking)
bool IS66_Write_DMA(is66_psram_t* p_dev, uint32_t address, const uint8_t* p_data, uint16_t size);
bool IS66_FastRead_DMA(is66_psram_t* p_dev, uint32_t address, uint8_t* p_data, uint16_t size, const uint8_t* p_dummy_byte);

// Inline function để quản lý Chip Select từ ngắt DMA ở lớp trên
void IS66_CS_Select(is66_psram_t* p_dev);

void IS66_CS_Deselect(is66_psram_t* p_dev);

#endif /* DEVICE_IS66WVS8M8BLL_IS66WVS8M8BLL_H_ */
