

#include "board.h"
#include "vswitch.h"
#include "pwm.h"
#include "string.h"

#include "stm32f4xx_ll_gpio.h"


#include "main.h"




uint32_t arr_channel_buf[MAX_VS_EVENTS];
uint32_t gpio_channel_buf[MAX_VS_EVENTS];

static volatile bool is_vs_sequence_done = false;
volatile uint16_t vs_event_count;

typedef struct {
    GPIO_TypeDef* Port;
    uint32_t HV_Pin;
    uint32_t LV_Pin;
} VS_Pole_Def_t;


const VS_Pole_Def_t VS_POLES = {
		.Port = V_SWITCH_HIN1_PORT,
		.HV_Pin = V_SWITCH_HIN1_PIN,
		.LV_Pin =  V_SWITCH_HIN2_PIN
};



void VS_Init(void){

    uint32_t ARR_VALUE = 47;
    uint32_t CCR_VALUE = (ARR_VALUE + 1) / 2;

    LL_TIM_OC_SetMode(V_SWITCH_LIN1_HANDLE, V_SWITCH_LIN1_CHANNEL, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(V_SWITCH_LIN1_HANDLE, V_SWITCH_LIN1_CHANNEL, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(V_SWITCH_LIN1_HANDLE, V_SWITCH_LIN1_CHANNEL);
    
    LL_TIM_SetAutoReload(V_SWITCH_LIN1_HANDLE, ARR_VALUE);
    LL_TIM_OC_SetCompareCH2(V_SWITCH_LIN1_HANDLE, CCR_VALUE);
    
    LL_TIM_CC_EnableChannel(V_SWITCH_LIN1_HANDLE, V_SWITCH_LIN1_CHANNEL);


    LL_TIM_OC_SetMode(V_SWITCH_LIN2_HANDLE,V_SWITCH_LIN2_CHANNEL, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(V_SWITCH_LIN2_HANDLE,V_SWITCH_LIN2_CHANNEL, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(V_SWITCH_LIN2_HANDLE,V_SWITCH_LIN2_CHANNEL);

    LL_TIM_SetAutoReload(V_SWITCH_LIN2_HANDLE, ARR_VALUE);
    LL_TIM_OC_SetCompareCH1(V_SWITCH_LIN2_HANDLE, CCR_VALUE);

    LL_TIM_CC_EnableChannel(V_SWITCH_LIN2_HANDLE, V_SWITCH_LIN2_CHANNEL);


    LL_TIM_SetOffStates(V_SWITCH_LIN1_HANDLE, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);
    LL_TIM_SetOffStates(V_SWITCH_LIN2_HANDLE, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);

    LL_TIM_DisableAutomaticOutput(V_SWITCH_LIN1_HANDLE);
    LL_TIM_DisableAutomaticOutput(V_SWITCH_LIN2_HANDLE);

    LL_TIM_SetPrescaler(V_SWITCH_LIN1_HANDLE, 299);
    LL_TIM_SetPrescaler(V_SWITCH_LIN2_HANDLE, 299);

    LL_TIM_EnableARRPreload(V_SWITCH_LIN1_HANDLE);
    LL_TIM_EnableARRPreload(V_SWITCH_LIN2_HANDLE);

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

    LL_TIM_DisableIT_UPDATE(V_SWITCH_LIN1_HANDLE);
    LL_TIM_DisableIT_UPDATE(V_SWITCH_LIN2_HANDLE);

    LL_TIM_SetUpdateSource(V_SWITCH_LIN1_HANDLE, LL_TIM_UPDATESOURCE_REGULAR);
    LL_TIM_SetUpdateSource(V_SWITCH_LIN2_HANDLE, LL_TIM_UPDATESOURCE_REGULAR);

    LL_TIM_GenerateEvent_UPDATE(V_SWITCH_LIN1_HANDLE);
    LL_TIM_GenerateEvent_UPDATE(V_SWITCH_LIN2_HANDLE);

    LL_TIM_EnableAllOutputs(V_SWITCH_LIN1_HANDLE);
    LL_TIM_EnableAllOutputs(V_SWITCH_LIN2_HANDLE);

    LL_TIM_EnableCounter(V_SWITCH_LIN1_HANDLE);
    LL_TIM_EnableCounter(V_SWITCH_LIN2_HANDLE);
}



static uint32_t VS_GetBSRR(vs_state_t state) {

    uint32_t hin = VS_POLES.HV_Pin;
    uint32_t lin = VS_POLES.LV_Pin;

    switch (state) {
    case VS_HIGH:

        return (lin << 16) | hin;

    case VS_LOW:

        return (hin << 16) | lin;

    default:

        return (hin << 16) | (lin << 16);
    }
}

void VS_Wave_Add(uint32_t bsrr, uint32_t dt) {

	gpio_channel_buf [vs_event_count] = bsrr;
    arr_channel_buf [vs_event_count]  = dt;

    vs_event_count++;
}

void VS_Pulse(uint32_t t_hv_on_us, uint32_t t_lv_on_us, uint32_t dead_us, uint8_t seq_count) {

    if(seq_count == 0) {
        VS_Wave_Add(VS_GetBSRR(VS_HIGH), 10);
    }

	VS_Wave_Add(VS_GetBSRR(VS_HIGH), t_hv_on_us);

	VS_Wave_Add(VS_GetBSRR(VS_FLOAT), dead_us);

	VS_Wave_Add(VS_GetBSRR(VS_LOW), t_lv_on_us);

	VS_Wave_Add(VS_GetBSRR(VS_FLOAT), dead_us);

}

void VS_Enable_Hv(void) {

    LL_GPIO_SetOutputPin(VS_POLES.Port, VS_POLES.HV_Pin); 
    LL_GPIO_ResetOutputPin(VS_POLES.Port, VS_POLES.LV_Pin);

}

void VS_Off(void) {
    LL_GPIO_ResetOutputPin(VS_POLES.Port, VS_POLES.HV_Pin);
    LL_GPIO_ResetOutputPin(VS_POLES.Port, VS_POLES.LV_Pin);
}

void DMA_VS_GPIO_Start(uint32_t len)
{
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);

    while(LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_1));

	LL_DMA_SetPeriphAddress(DMA2,LL_DMA_STREAM_1, (uint32_t) &GPIOB->BSRR);

    LL_DMA_SetMemoryAddress(DMA2,LL_DMA_STREAM_1,(uint32_t)gpio_channel_buf);

	LL_DMA_SetDataLength(DMA2,LL_DMA_STREAM_1, len);

	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_1);

	LL_DMA_EnableIT_TC(DMA2,LL_DMA_STREAM_1);
}

void DMA_VS_ARR_Start(uint32_t len)
{
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_7);

    while(LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_7));

	LL_DMA_SetPeriphAddress(DMA2,LL_DMA_STREAM_7, (uint32_t) &TIM8->ARR);

	LL_DMA_SetMemoryAddress(DMA2,LL_DMA_STREAM_7, (uint32_t) arr_channel_buf);

	LL_DMA_SetDataLength(DMA2,LL_DMA_STREAM_7, len);

	LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_7);
}

bool VS_Is_Phase_Done(void) {
    return is_vs_sequence_done;
}

void VS_Clear_Sequence(void) {
    vs_event_count = 0;

    memset(arr_channel_buf, 0, sizeof(arr_channel_buf));
    memset(gpio_channel_buf, 0, sizeof(gpio_channel_buf));
}

void VS_Start(void)
{
    is_vs_sequence_done = false;

    DMA_VS_GPIO_Start(vs_event_count);
    DMA_VS_ARR_Start(vs_event_count);

    LL_TIM_SetCounter(TIM8, 0);
    LL_TIM_DisableARRPreload(TIM8);

    TIM8->ARR  = 489;
    TIM8->CCR4 = 1;

    LL_TIM_EnableARRPreload(TIM8);


    LL_TIM_EnableDMAReq_UPDATE(TIM8);
    LL_TIM_EnableDMAReq_CC4(TIM8);

    LL_TIM_EnableCounter(TIM8);
}

void DMA2_Stream1_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC1(DMA2))
    {
        LL_DMA_ClearFlag_TC1(DMA2);

        LL_TIM_DisableCounter(TIM8);

        LL_TIM_DisableDMAReq_UPDATE(TIM8);

        LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_1);

        is_vs_sequence_done = true;
    }
}

void DMA2_Stream7_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC7(DMA2))
    {
        LL_DMA_ClearFlag_TC7(DMA2);

        LL_TIM_DisableCounter(TIM8);

        LL_TIM_DisableDMAReq_CC4(TIM8);

        LL_DMA_DisableStream( DMA2,LL_DMA_STREAM_7);

        is_vs_sequence_done = true;
    }
}




