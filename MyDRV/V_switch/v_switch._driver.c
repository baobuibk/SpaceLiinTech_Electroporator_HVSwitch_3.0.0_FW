/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "v_switch_driver.h"
#include "app.h"
#include "stm32f4xx_ll_gpio.h"

int hihi = 0;

#define ARR_BUFF_SIZE 40

PWM_TypeDef V_Switch_HV_PWM =
{
    .TIMx       =   V_SWITCH_LIN1_HANDLE,
    .Channel    =   V_SWITCH_LIN1_CHANNEL,
    .Prescaler  =   300,
    .Mode       =   LL_TIM_OCMODE_FORCED_INACTIVE,
    .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
    .Duty       =   0,
    .Freq       =   0,
};
PWM_TypeDef V_Switch_LV_PWM =
{
    .TIMx       =   V_SWITCH_LIN2_HANDLE,
    .Channel    =   V_SWITCH_LIN2_CHANNEL,
    .Prescaler  =   300,
    .Mode       =   LL_TIM_OCMODE_FORCED_INACTIVE,
    .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
    .Duty       =   0,
    .Freq       =   0,
};

V_Switch_typdef V_Switch_HV =
{
    .Port               = V_SWITCH_HIN1_PORT,
    .Pin                = V_SWITCH_HIN1_PIN,
    .PWM                = &V_Switch_HV_PWM,
    .is_on              = false,
};

V_Switch_typdef V_Switch_LV =
{
    .Port               = V_SWITCH_HIN2_PORT,
    .Pin                = V_SWITCH_HIN2_PIN,
    .PWM                = &V_Switch_LV_PWM,
    .is_on              = false,
};


uint32_t ARR_buff[40];
volatile uint8_t arr_index = 0;

volatile V_Switch_state switch_state = 0;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
__STATIC_INLINE void VS_Set_Duty(PWM_TypeDef *PWMx, uint32_t _Duty, bool apply_now);
__STATIC_INLINE void VS_Set_OC(PWM_TypeDef *PWMx, uint32_t _OC, bool apply_now);
__STATIC_INLINE void VS_Set_Freq(PWM_TypeDef *PWMx, uint32_t _Freq, bool apply_now);
__STATIC_INLINE void VS_Set_ARR(PWM_TypeDef *PWMx, uint32_t _ARR, bool apply_now);

/* :::::::::: V_Switch Task Init :::::::: */
void V_Switch_Driver_Init(void)
{
//	LL_TIM_SetPrescaler(TIM2, 7199);
//
//	LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
//
////	LL_TIM_SetAutoReload(TIM2, 100);
//
//	LL_TIM_EnableARRPreload(TIM2);
//
//	LL_TIM_ClearFlag_UPDATE(TIM2);
//
//	LL_TIM_EnableIT_UPDATE(TIM2);
//
//	LL_TIM_DisableCounter(TIM2);


    // V SWITCH HV INIT
    LL_TIM_OC_SetMode(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Channel,  LL_TIM_OCMODE_FORCED_INACTIVE);
    LL_TIM_OC_SetPolarity(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Channel, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Channel);
    LL_TIM_CC_EnableChannel(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Channel);

    // V SWITCH LV INIT
    LL_TIM_OC_SetMode(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Channel,  LL_TIM_OCMODE_FORCED_INACTIVE);
    LL_TIM_OC_SetPolarity(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Channel, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Channel);
    LL_TIM_CC_EnableChannel(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Channel);

    LL_TIM_SetOffStates(V_Switch_HV.PWM->TIMx, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);
    LL_TIM_SetOffStates(V_Switch_LV.PWM->TIMx, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);

    LL_TIM_DisableAutomaticOutput(V_Switch_HV.PWM->TIMx);
    LL_TIM_DisableAutomaticOutput(V_Switch_LV.PWM->TIMx);

    LL_TIM_EnableAllOutputs(V_Switch_HV.PWM->TIMx);
    LL_TIM_EnableAllOutputs(V_Switch_LV.PWM->TIMx);

    LL_TIM_SetPrescaler(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Prescaler);
    LL_TIM_SetPrescaler(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Prescaler);

    LL_TIM_EnableARRPreload(V_Switch_HV.PWM->TIMx);
    LL_TIM_EnableARRPreload(V_Switch_LV.PWM->TIMx);

    LL_GPIO_ResetOutputPin(V_Switch_HV.Port, V_Switch_HV.Pin);
    LL_GPIO_ResetOutputPin(V_Switch_LV.Port, V_Switch_LV.Pin);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

    LL_TIM_DisableIT_UPDATE(V_Switch_HV.PWM->TIMx);
    LL_TIM_DisableIT_UPDATE(V_Switch_LV.PWM->TIMx);

    LL_TIM_SetUpdateSource(V_Switch_HV.PWM->TIMx, LL_TIM_UPDATESOURCE_REGULAR);
    LL_TIM_SetUpdateSource(V_Switch_LV.PWM->TIMx, LL_TIM_UPDATESOURCE_REGULAR);

    LL_TIM_EnableCounter(V_Switch_HV.PWM->TIMx);
    LL_TIM_EnableCounter(V_Switch_LV.PWM->TIMx);
}

void V_Switch_Set_Mode(V_Switch_mode SetMode)
{
//    LL_TIM_DisableCounter(V_Switch_HV.PWM->TIMx);
//    LL_TIM_DisableCounter(V_Switch_LV.PWM->TIMx);

    switch (SetMode)
    {
    case V_SWITCH_MODE_HV_ON:

        LL_TIM_OC_SetMode(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Channel, LL_TIM_OCMODE_PWM2);
        VS_Set_Freq(V_Switch_HV.PWM, 5000, 0);
        VS_Set_Duty(V_Switch_HV.PWM, 50, 0);

        LL_GPIO_SetOutputPin(V_Switch_HV.Port, V_Switch_HV.Pin);

        switch_state = V_SWITCH_STATE_ON;
        V_Switch_HV.is_on = true;
        V_Switch_LV.is_on = false;

        break;

    case V_SWITCH_MODE_LV_ON:

        LL_TIM_OC_SetMode(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Channel, LL_TIM_OCMODE_PWM2);
        VS_Set_Freq(V_Switch_LV.PWM, 5000, 0);
        VS_Set_Duty(V_Switch_LV.PWM, 50, 0);

        LL_GPIO_SetOutputPin(V_Switch_LV.Port, V_Switch_LV.Pin);

        switch_state = V_SWITCH_STATE_ON;
        V_Switch_LV.is_on = true;
        V_Switch_HV.is_on = false;

        break;

    case V_SWITCH_MODE_ALL_OFF:
        LL_TIM_OC_SetMode(V_Switch_LV.PWM->TIMx, V_Switch_LV.PWM->Channel, LL_TIM_OCMODE_FORCED_INACTIVE);
        LL_GPIO_ResetOutputPin(V_Switch_LV.Port, V_Switch_LV.Pin);

        LL_TIM_OC_SetMode(V_Switch_HV.PWM->TIMx, V_Switch_HV.PWM->Channel, LL_TIM_OCMODE_FORCED_INACTIVE);
        LL_GPIO_ResetOutputPin(V_Switch_HV.Port, V_Switch_HV.Pin);


        switch_state = V_SWITCH_STATE_DEADTIME;

        break;

    default:
        break;
    }

//    LL_TIM_GenerateEvent_UPDATE(V_Switch_HV.PWM->TIMx);
//    LL_TIM_GenerateEvent_UPDATE(V_Switch_LV.PWM->TIMx);
//
//    LL_TIM_EnableCounter(V_Switch_HV.PWM->TIMx);
//    LL_TIM_EnableCounter(V_Switch_LV.PWM->TIMx);
}

void V_Switch_Change_Phase_Calc(uint32_t HV_on_us, uint32_t LV_on_us, uint32_t dead_us, uint8_t seq_idx)
{
	uint8_t fill_index = seq_idx * 4;

    ARR_buff[fill_index + 0] = HV_on_us;
    ARR_buff[fill_index + 1] = dead_us;
    ARR_buff[fill_index + 2] = LV_on_us;
    ARR_buff[fill_index + 3] = dead_us;
}

void V_Switch_Start(void)
{

    arr_index = 0;

    LL_TIM_SetCounter(TIM2, 0);
//    // ← Reset counter về 0
//    LL_TIM_SetAutoReload(TIM2, ARR_buff[arr_index++]);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetAutoReload(TIM2, 500);
    LL_TIM_CC_EnablePreload(TIM2);

    LL_TIM_ClearFlag_UPDATE(TIM2);                         // ← Clear UIF trước khi enable IT

    V_Switch_Set_Mode(V_SWITCH_MODE_HV_ON);

    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);

}



void V_Switch_ISR(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM2) == 1)
    {
        LL_TIM_ClearFlag_UPDATE(TIM2);


        if (arr_index < ARR_BUFF_SIZE)
        {
            LL_TIM_SetAutoReload(TIM2, ARR_buff[arr_index++]);
        }
        else
        {
            LL_TIM_DisableCounter(TIM2);
            LL_TIM_DisableIT_UPDATE(TIM2);
            V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
            return;
        }


        switch(switch_state)
        {
        case V_SWITCH_STATE_ON:
            V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
            break;

        case V_SWITCH_STATE_DEADTIME:
            if (V_Switch_HV.is_on == true)
                V_Switch_Set_Mode(V_SWITCH_MODE_LV_ON);
            else if (V_Switch_LV.is_on == true)
                V_Switch_Set_Mode(V_SWITCH_MODE_HV_ON);
            break;
        }
    }
}

void V_Switch_Clear_BuffARR(void)
{
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_DisableIT_UPDATE(TIM2);

    for (uint8_t i = 0; i < ARR_BUFF_SIZE; i++)
    {
        ARR_buff[i] = 5000;
    }
    arr_index = 0;
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
__STATIC_INLINE void VS_Set_Duty(PWM_TypeDef *PWMx, uint32_t _Duty, bool apply_now)
{
    LL_TIM_DisableUpdateEvent(PWMx->TIMx);

    // Limit the duty to 100
    if (_Duty > 100)
      return;

    // Set PWM DUTY for channel 1
    PWMx->Duty = (PWMx->Freq * (_Duty / 100.0));
    switch (PWMx->Channel)
    {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(PWMx->TIMx, PWMx->Duty);
        break;

    default:
        break;
    }

    LL_TIM_EnableUpdateEvent(PWMx->TIMx);

    if(apply_now == 1)
    {
        if (LL_TIM_IsEnabledIT_UPDATE(PWMx->TIMx))
        {
            LL_TIM_DisableIT_UPDATE(PWMx->TIMx);
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
            LL_TIM_EnableIT_UPDATE(PWMx->TIMx);
        }
        else
        {
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
        }
    }
}

__STATIC_INLINE void VS_Set_OC(PWM_TypeDef *PWMx, uint32_t _OC, bool apply_now)
{
    LL_TIM_DisableUpdateEvent(PWMx->TIMx);

    // Set PWM DUTY for channel 1
    PWMx->Duty = _OC;
    switch (PWMx->Channel)
    {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(PWMx->TIMx, PWMx->Duty);
        break;

    default:
        break;
    }

    LL_TIM_EnableUpdateEvent(PWMx->TIMx);

    if(apply_now == 1)
    {
        if (LL_TIM_IsEnabledIT_UPDATE(PWMx->TIMx))
        {
            LL_TIM_DisableIT_UPDATE(PWMx->TIMx);
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
            LL_TIM_EnableIT_UPDATE(PWMx->TIMx);
        }
        else
        {
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
        }
    }
}

__STATIC_INLINE void VS_Set_Freq(PWM_TypeDef *PWMx, uint32_t _Freq, bool apply_now)
{
    LL_TIM_DisableUpdateEvent(PWMx->TIMx);

    // Set PWM FREQ
    PWMx->Freq = __LL_TIM_CALC_ARR(APB1_TIMER_CLK, LL_TIM_GetPrescaler(PWMx->TIMx), _Freq);
    LL_TIM_SetAutoReload(PWMx->TIMx, PWMx->Freq);

    LL_TIM_EnableUpdateEvent(PWMx->TIMx);

    if(apply_now == 1)
    {
        if (LL_TIM_IsEnabledIT_UPDATE(PWMx->TIMx))
        {
            LL_TIM_DisableIT_UPDATE(PWMx->TIMx);
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
            LL_TIM_EnableIT_UPDATE(PWMx->TIMx);
        }
        else
        {
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
        }
    }
}

__STATIC_INLINE void VS_Set_ARR(PWM_TypeDef *PWMx, uint32_t _ARR, bool apply_now)
{
    LL_TIM_DisableUpdateEvent(PWMx->TIMx);

    // Set PWM FREQ
    PWMx->Freq = _ARR;
    LL_TIM_SetAutoReload(PWMx->TIMx, PWMx->Freq);

    LL_TIM_EnableUpdateEvent(PWMx->TIMx);

    if(apply_now == 1)
    {
        if (LL_TIM_IsEnabledIT_UPDATE(PWMx->TIMx))
        {
            LL_TIM_DisableIT_UPDATE(PWMx->TIMx);
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
            LL_TIM_EnableIT_UPDATE(PWMx->TIMx);
        }
        else
        {
            LL_TIM_GenerateEvent_UPDATE(PWMx->TIMx);
        }
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
