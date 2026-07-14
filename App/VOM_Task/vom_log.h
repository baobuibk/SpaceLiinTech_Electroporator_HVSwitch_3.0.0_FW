/*
 * vom_log.h
 *
 *  Created on: Jul 10, 2026
 *      Author: PV
 */

#ifndef VOM_TASK_VOM_LOG_H_
#define VOM_TASK_VOM_LOG_H_

#include "spi.h"
#include "IS66WVS8M8BLL.h"

#define VOM_LOG_SAMPLE_SIZE_MAX 254

/*---------------------------- VOM LOG ----------------------------*/
/*
* Việc đọc ghi vào psram sẽ theo burst length mặc định và 1024 byte 1 lần ghi
* Tổ chức ghi theo block, mỗi block sẽ gồm:
    + 254 mẫu gồm dòng điện và điện áp (1016 bytes)
    + Thông tin quản lý block: thời gian lấy mẫu, thứ tự block, số mẫu có trong block (8 bytes)
*/

typedef struct {
    uint16_t voltage; // x100 (10mV)
    uint16_t current; // x1000 (1mA)
} vom_data_sample_t;

typedef struct {
    uint32_t start_timestamp_us; 
    uint16_t block_index;        
    uint16_t sample_count;      
    vom_data_sample_t samples[254];  
} vom_data_block_t;

typedef struct {
    uint32_t            psram_address;
    vom_data_block_t    *p_vom_data_block;
}vom_log_handle_t;

extern is66_psram_t vom_is66w_dev;
extern vom_log_handle_t vom_log_handle;

extern vom_data_block_t VOM_DATA_BLOCK_A;
extern vom_data_block_t VOM_DATA_BLOCK_B;

extern vom_data_block_t VOM_DEBUG_BLOCK;


void VOM_psram_init(void);

void VOM_Log_Stop(void);

void VOM_Log_Read_Block_Debug(uint16_t block_index);

void VOM_Log_DMA_Transmit_Complete_ISR(void);
void VOM_Log_DMA_Receive_Complete_ISR(void);

#endif /* VOM_TASK_VOM_LOG_H_ */
