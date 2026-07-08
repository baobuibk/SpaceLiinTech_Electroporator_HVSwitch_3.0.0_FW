/*
 * accel_pulsing_task.h
 *
 *  Created on: Jul 8, 2026
 *      Author: PV
 */

#ifndef ACCEL_PULSING_TASK_ACCEL_PULSING_TASK_H_
#define ACCEL_PULSING_TASK_ACCEL_PULSING_TASK_H_

#include "stdint.h"
#include "stdbool.h"

typedef enum{
    ACCEL_PULSING_STATE_IDLE = 0,
    ACCEL_PULSING_STATE_INITIALIZE,
    ACCEL_PULSING_STATE_WAITING_TRIGGER,
    ACCEL_PULSING_STATE_PULSING,
    ACCEL_PULSING_STATE_ERROR
} Accel_Pulsing_Task_State;


#endif /* ACCEL_PULSING_TASK_ACCEL_PULSING_TASK_H_ */
