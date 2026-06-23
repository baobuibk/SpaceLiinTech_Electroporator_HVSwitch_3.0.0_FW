/*
 * V_switch_driver.h
 *
 *  Created on: Jun 17, 2026
 *      Author: PV
 */

#ifndef V_SWITCH_V_SWITCH_DRIVER_H_
#define V_SWITCH_V_SWITCH_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#include "board.h"

#include "pwm.h"

typedef enum _V_Switch_mode_typedef_
{
    V_SWITCH_MODE_HV_ON,
    V_SWITCH_MODE_LV_ON,
    V_SWITCH_MODE_ALL_OFF,

}V_Switch_mode;

typedef enum _V_Switch_state_typedef_
{
    V_SWITCH_STATE_ON = 0,
	V_SWITCH_STATE_DEADTIME

}V_Switch_state;

typedef struct _V_Switch_typdef_
{
    GPIO_TypeDef    *Port;
    uint32_t        Pin;
    PWM_TypeDef     *PWM;
    bool            is_on;

}V_Switch_typdef;

extern V_Switch_typdef V_Switch_HV;
extern V_Switch_typdef V_Switch_LV;

void V_Switch_Driver_Init(void);
void V_Switch_Start(void);
void V_Switch_Change_Phase_Calc(uint32_t HV_on_us, uint32_t LV_on_us, uint32_t dead_us, uint8_t seq_idx);
void V_Switch_Clear_BuffARR(void);
void V_Switch_Set_Mode(V_Switch_mode SetMode);

void V_Switch_ISR(void);

#endif /* V_SWITCH_V_SWITCH_DRIVER_H_ */
