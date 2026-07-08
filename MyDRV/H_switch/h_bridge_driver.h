#ifndef H_BRIDGE_DRIVER_H_
#define H_BRIDGE_DRIVER_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"
#include "v_switch_driver.h"

#define MAX_EVENTS 3200

typedef struct
{
    uint32_t gpio_bsrr;
    uint32_t dt;
} event_t;

typedef enum
{
    HB_FLOAT = 0,
    HB_HIGH,
    HB_LOW
} hb_state_t;

void HB_Set_Pole_Pos_Manual(uint8_t ch, uint8_t mass_ch);

uint32_t HB_Pulse(uint8_t ch, uint8_t mass_ch, uint32_t ton_us, uint32_t toff_us, uint32_t dead_us, uint8_t count) ;

uint32_t HB_Delay(uint8_t ch_1, uint8_t ch_2, uint32_t delay_us);
void HB_Off(void);
void HB_Start(void);
void HB_Stop_Priority(void);

bool HB_Is_Phase_Done(void);
void HB_Clear_Sequence(void);

#endif /* H_BRIDGE_DRIVER_H_ */
