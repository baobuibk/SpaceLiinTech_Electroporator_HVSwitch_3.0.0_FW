/*
 * impedance_task.h
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#ifndef IMPEDANCE_TASK_IMPEDANCE_TASK_H_
#define IMPEDANCE_TASK_IMPEDANCE_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

#include "stm32f4xx_ll_gpio.h"

typedef enum
{
    IMPEDANCE_TASK_STATE_IDLE = 0,
    IMPEDANCE_TASK_STATE_INITIAL,
    IMPEDANCE_TASK_STATE_SET_CHARGE,
    IMPEDANCE_TASK_STATE_CHARGING,
    IMPEDANCE_TASK_STATE_CACULATE,

} impedance_task_state_t;

typedef struct{
    uint8_t     pos_pole;
    uint8_t     neg_pole;
}impedance_pole;


extern bool                    is_impedance_task_enable;
extern impedance_task_state_t  impedance_task_state;
extern uint16_t                impedance_task_volt_charged;
extern impedance_pole          impedance_select_pole;
extern uint32_t				   im_time_out;

void impedance_task (void*);

#endif /* IMPEDANCE_TASK_IMPEDANCE_TASK_H_ */
