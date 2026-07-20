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
#include "INA229.h"

#include "xbee_cmd_task.h"
#include "board.h"
#include "stm32f4xx_ll_tim.h"

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"



#define VOM_LOG_SAMPLE_SIZE_MAX 254
#define SPI_BUFFER_SIZE 10

#define SHUNT_RESISTOR_OHMS 0.014f
#define MAX_EXPECTED_AMPS   10.0f


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

typedef struct {

    double bus_voltage;
    double shunt_voltage;
    double temperature;
    double current;
    double power;
    double energy;
    double charge;

    float current_limit; // Current limit in Amperes
    bool  over_voltage_flag; // Over-voltage flag
    bool  over_current_flag; // Over-current flag

} vom_data_t;


extern is66_psram_t vom_is66w_dev;
extern ina229_t vom_ina229_dev;

extern vom_data_t vom_data; 
extern vom_data_block_t VOM_DATA_BLOCK_A;
extern vom_data_block_t VOM_DATA_BLOCK_B;

extern vom_log_handle_t vom_log_handle;

void VOM_Psram_Init(void);
void VOM_INA229_Init(void);
void VOM_Timbase_Sampling_Init(void);

void VOM_Log_Read_Block_Debug(uint16_t block_index);
void VOM_Log_Get_HexDump(uint32_t start_address, uint32_t length);

void VOM_Set_Current_Limit(float current_limit_amps);
float VOM_Get_Current_Limit(void);
bool VOM_Get_OVC_Flag(void);
void VOM_Reset_OVC_Flag(void);

void VOM_Sampling_SetFrequency(uint32_t sampling_freq_hz);


#endif /* VOM_TASK_VOM_LOG_H_ */
