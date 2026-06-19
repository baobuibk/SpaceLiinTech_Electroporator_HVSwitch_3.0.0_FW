#ifndef H_BRIDGE_DRIVER_H_
#define H_BRIDGE_DRIVER_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"
#include "v_switch_driver.h"

#define HB_PULSE_STATE_PERIOD		2000 //MS



typedef struct
{
    GPIO_TypeDef* Port;

    uint32_t        Pos_HIN_Pin;
    uint32_t        Pos_LIN_Pin;
    uint32_t        Neg_HIN_Pin;
    uint32_t        Neg_LIN_Pin;
} HBridge_Config_t;



//extern bool is_sequence_done;

void HB_Init(HBridge_Config_t *p_cfg);


void HB_Force_Safe_State(void);


uint16_t HB_Build_Pulse_Buff(uint16_t on_time_ms, uint16_t off_time_ms);


uint16_t HB_Build_Delay_Buff(uint16_t delay_time_ms);


void HB_Start_DMA_Sequence(uint16_t buffer_length, uint32_t num_pulse);


void HB_Pulse_DMA_ISR(void);


bool HB_Is_Phase_Done(void);


#endif /* H_BRIDGE_DRIVER_H_ */
