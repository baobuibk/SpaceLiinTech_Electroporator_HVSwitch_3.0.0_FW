/*
 * vom_task.h
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#ifndef VOM_TASK_VOM_TASK_H_
#define VOM_TASK_VOM_TASK_H_


#include "spi.h"

#include "board.h"
#include "h_bridge_task.h"
#include "scheduler.h"
#include "xbee_cmd_task.h"

#include "vom_log.h"
#include "app.h"

#include "stm32f4xx_ll_dma.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


typedef enum {
    VOM_SAMPLING_VOLTAGE = 0,
    VOM_SAMPLING_CURRENT,
} vom_sampling_state_t;


void VOM_Init(void);

void VOM_Sampling_Start(void);
void VOM_Sampling_Stop(void);

void VOM_Task (void*);

void VOM_DMA_Receive_Complete_ISR(void);
void VOM_DMA_Trasmit_Complete_ISR(void);
void VOM_Log_DMA_Transmit_Complete_ISR(void);
void VOM_Log_DMA_Receive_Complete_ISR(void);
void VOM_TIM_Sampling_Trigger_ISR(void);
void VOM_OVC_Handle_ISR(void);

#endif /* VOM_TASK_VOM_TASK_H_ */
