#include "board.h"
#include "h_bridge_driver.h"


#include "main.h"

#include "string.h"
// event_t event_buf[MAX_EVENTS];
uint32_t arr_buf[MAX_EVENTS];
uint32_t gpio_buf[MAX_EVENTS];

static volatile bool is_sequence_done = false;

typedef struct {
    GPIO_TypeDef* Port;
    uint32_t HIN_Pin;
    uint32_t LIN_Pin;
} HB_Pole_Def_t;

const HB_Pole_Def_t HB_POLES[8] = {
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN0_PIN, H_BRIDGE_LIN0_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN1_PIN, H_BRIDGE_LIN1_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN2_PIN, H_BRIDGE_LIN2_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN3_PIN, H_BRIDGE_LIN3_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN4_PIN, H_BRIDGE_LIN4_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN5_PIN, H_BRIDGE_LIN5_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN6_PIN, H_BRIDGE_LIN6_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN7_PIN, H_BRIDGE_LIN7_PIN }
};

volatile uint16_t event_count;

static uint32_t HB_GetBSRR(uint8_t ch, uint8_t mass_ch, hb_state_t state) {

    uint32_t hin = HB_POLES[ch].HIN_Pin;
    uint32_t lin = HB_POLES[ch].LIN_Pin;

    uint32_t mass_hin = HB_POLES[mass_ch].HIN_Pin;
    uint32_t mass_lin = HB_POLES[mass_ch].LIN_Pin;

	switch (state) {
	case HB_HIGH:
		return (lin << 16) | (mass_hin << 16) | mass_lin | hin;

	case HB_LOW:
		return (hin << 16) | (mass_hin << 16) | mass_lin | lin;

	default:
		return (hin << 16) | (mass_hin << 16) | (lin << 16);
	}
}

void Wave_Add(uint32_t bsrr, uint32_t dt) {

    gpio_buf[event_count] = bsrr;
    arr_buf[event_count]  = dt;

    event_count++;
}

uint32_t HB_Pulse(uint8_t ch, uint8_t mass_ch, uint32_t ton_us, uint32_t toff_us, uint32_t dead_us, uint8_t count) {

    uint32_t toltal_time = 0;

    for(uint8_t i = 0; i < count; i++){

        Wave_Add(HB_GetBSRR(ch, mass_ch, HB_FLOAT), dead_us);

        Wave_Add(HB_GetBSRR(ch, mass_ch, HB_HIGH), ton_us);

		Wave_Add(HB_GetBSRR(ch, mass_ch, HB_FLOAT), dead_us);

		Wave_Add(HB_GetBSRR(ch, mass_ch, HB_LOW), toff_us);

        toltal_time += (ton_us + 1) + (toff_us + 1) + dead_us*2;
    }
    
    return toltal_time; //This function return total time in us
}

uint32_t HB_Delay(uint8_t ch_1, uint8_t ch_2, uint32_t delay_us) {

	Wave_Add(HB_GetBSRR(ch_1, ch_2, HB_LOW), delay_us);

    return delay_us;
}

void HB_Off(void){

	GPIOC->BSRR = 0xFFFF0000;
}

void DMA_GPIO_Start(uint32_t len)
{
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_5);

    while(LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_5));

	LL_DMA_SetPeriphAddress(DMA2,LL_DMA_STREAM_5, (uint32_t) &GPIOC->BSRR);

    LL_DMA_SetMemoryAddress(DMA2,LL_DMA_STREAM_5,(uint32_t)gpio_buf);

	LL_DMA_SetDataLength(DMA2,LL_DMA_STREAM_5, len);

	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_5);

	LL_DMA_EnableIT_TC(DMA2,LL_DMA_STREAM_5);
}

void DMA_ARR_Start(uint32_t len)
{
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_4);

    while(LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_4));

	LL_DMA_SetPeriphAddress(DMA2,LL_DMA_STREAM_4, (uint32_t) &TIM1->ARR);

	LL_DMA_SetMemoryAddress(DMA2,LL_DMA_STREAM_4, (uint32_t) arr_buf);

	LL_DMA_SetDataLength(DMA2,LL_DMA_STREAM_4, len);

	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_4);
}

bool HB_Is_Phase_Done(void) {
    return is_sequence_done;
}

void HB_Clear_Sequence(void) {
    event_count = 0;

    memset(arr_buf, 0, sizeof(arr_buf));
    memset(gpio_buf, 0, sizeof(gpio_buf));
}

void HB_Start(void)
{
    is_sequence_done = false;

    DMA_GPIO_Start(event_count);

    DMA_ARR_Start(event_count);
    LL_TIM_SetCounter(TIM1, 0);

    LL_TIM_DisableARRPreload(TIM1);

    TIM1->ARR  = 499;
    TIM1->CCR4 = 1;

    LL_TIM_EnableARRPreload(TIM1);

    LL_TIM_EnableDMAReq_UPDATE(TIM1);
    LL_TIM_EnableDMAReq_CC4(TIM1);

    LL_TIM_EnableCounter(TIM1);
}

void DMA2_Stream5_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC5(DMA2))
    {
        LL_DMA_ClearFlag_TC5(DMA2);

        LL_TIM_DisableCounter(TIM1);

        LL_TIM_DisableDMAReq_UPDATE(TIM1);
//        LL_TIM_DisableDMAReq_CC4(TIM1);

        LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_5);

//        LL_DMA_DisableStream( DMA2,LL_DMA_STREAM_4);

        is_sequence_done = true;
    }
}

void DMA2_Stream4_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC4(DMA2))
    {
        LL_DMA_ClearFlag_TC4(DMA2);

        LL_TIM_DisableCounter(TIM1);

//        LL_TIM_DisableDMAReq_UPDATE(TIM1);
        LL_TIM_DisableDMAReq_CC4(TIM1);

//        LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_5);

        LL_DMA_DisableStream( DMA2,LL_DMA_STREAM_4);

        is_sequence_done = true;
    }
}

