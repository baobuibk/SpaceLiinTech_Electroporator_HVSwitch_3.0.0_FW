/*
 * v_switch_task.h
 *
 *  Created on: Jun 17, 2026
 *      Author: PV
 */

#ifndef V_SWITCH_TASK_V_SWITCH_TASK_H_
#define V_SWITCH_TASK_V_SWITCH_TASK_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    V_SWITCH_STOP_STATE,
    V_SWITCH_C1_STATE,
    V_SWITCH_C2_STATE,
} V_Switch_state_typdef;

extern V_Switch_state_typdef Channel_Set;
extern bool is_v_switch_enable;

/* :::::::::: V_Switch Task Init :::::::: */
void V_Switch_Task_Init(void);

/* :::::::::: V_Switch Task ::::::::::::: */
void V_Switch_Task(void*);

/* ::::V_Switch 1 Interupt Handle:::: */
void V_Switch_1_Interupt_Handle();

/* ::::V_Switch 2 Interupt Handle:::: */
void V_Switch_2_Interupt_Handle();

#endif /* V_SWITCH_TASK_V_SWITCH_TASK_H_ */
