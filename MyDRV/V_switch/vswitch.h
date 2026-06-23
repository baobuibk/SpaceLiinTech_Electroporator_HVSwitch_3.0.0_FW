/*
 * vswitch.h
 *
 *  Created on: Jun 22, 2026
 *      Author: PV
 */

#ifndef V_SWITCH_VSWITCH_H_
#define V_SWITCH_VSWITCH_H_

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"



#define MAX_VS_EVENTS 100

typedef struct
{
    uint32_t gpio_bsrr;
    uint32_t dt;
} vs_event_t;

typedef enum
{
    VS_FLOAT = 0,
    VS_HIGH,
    VS_LOW
} vs_state_t;

void VS_Init(void);
void VS_Pulse(uint32_t t_hv_on_us, uint32_t t_lv_on_us, uint32_t dead_us, uint8_t seq_count);

void VS_Off(void);

void VS_Start(void);

bool VS_Is_Phase_Done(void);
void VS_Clear_Sequence(void);


#endif /* V_SWITCH_VSWITCH_H_ */
