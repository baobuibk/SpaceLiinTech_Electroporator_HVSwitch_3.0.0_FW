#include "board.h"
#include "h_bridge_driver.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t HB_Pulse_Port_Buff[HB_PULSE_STATE_PERIOD];


static HBridge_Config_t     current_hb_cfg;
static HBridge_Config_t* 	p_hb_state = NULL;

static volatile uint32_t    pulse_cnt = 0;
static volatile bool        is_sequence_done = true;
static uint16_t             current_buff_len = 0;

void HB_Init(HBridge_Config_t *p_cfg) {
    if (p_cfg == NULL) return;

    current_hb_cfg = *p_cfg;
    p_hb_state = &current_hb_cfg;

    HB_Force_Safe_State();

    LL_DMA_DisableStream(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE);
    LL_TIM_EnableDMAReq_UPDATE(H_BRIDGE_TIM_HANDLE);
}

void HB_Force_Safe_State(void) {
    if (p_hb_state == NULL) return;

    uint32_t safe_val = (p_hb_state->Pos_HIN_Pin << 16) |
                        (p_hb_state->Pos_LIN_Pin << 16) |
                        (p_hb_state->Neg_HIN_Pin << 16) |
                        (p_hb_state->Neg_LIN_Pin << 16);

    p_hb_state->Port->BSRR = safe_val;
}

uint16_t HB_Build_Pulse_Buff(uint16_t on_time_ms, uint16_t off_time_ms) {
    if (p_hb_state == NULL) return 0;

    uint16_t total_time = on_time_ms + off_time_ms;
    if (total_time > HB_PULSE_STATE_PERIOD) return 0;

    uint32_t state_on_val = 0;
    uint32_t state_off_val = 0;

    state_on_val |= (p_hb_state->Pos_HIN_Pin) | (p_hb_state->Neg_LIN_Pin);
    state_on_val |= (p_hb_state->Pos_LIN_Pin << 16) | (p_hb_state->Neg_HIN_Pin << 16);

    state_off_val |= (p_hb_state->Neg_HIN_Pin) | (p_hb_state->Pos_LIN_Pin);
    state_off_val |= (p_hb_state->Neg_LIN_Pin << 16) | (p_hb_state->Pos_HIN_Pin << 16);

    for(uint16_t time_cnt = 0; time_cnt < total_time; time_cnt++) {
        if (time_cnt < on_time_ms)
            HB_Pulse_Port_Buff[time_cnt] = state_on_val;
        else
            HB_Pulse_Port_Buff[time_cnt] = state_off_val;
    }

    return total_time;
}

uint16_t HB_Build_Delay_Buff(uint16_t delay_time_ms) {
    if (p_hb_state == NULL || delay_time_ms > HB_PULSE_STATE_PERIOD) return 0;

    uint32_t state_idle_val = 0;

    state_idle_val |= (p_hb_state->Pos_HIN_Pin << 16) | (p_hb_state->Pos_LIN_Pin << 16) |
                      (p_hb_state->Neg_HIN_Pin << 16) | (p_hb_state->Neg_LIN_Pin << 16);

    for(uint16_t time_cnt = 0; time_cnt < delay_time_ms; time_cnt++) {
        HB_Pulse_Port_Buff[time_cnt] = state_idle_val;
    }

    return delay_time_ms;
}

void HB_Start_DMA_Sequence(uint16_t buffer_length, uint32_t num_pulse) {
    if (p_hb_state == NULL || buffer_length == 0) return;

    current_buff_len = buffer_length;
    is_sequence_done = false;

    pulse_cnt = num_pulse;

    LL_TIM_DisableCounter(H_BRIDGE_TIM_HANDLE);
    LL_DMA_DisableStream(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE);


    LL_DMA_SetPeriphAddress(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE, (uint32_t)&p_hb_state->Port->BSRR);
    LL_DMA_SetMemoryAddress(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE, (uint32_t)HB_Pulse_Port_Buff);
    LL_DMA_SetDataLength(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE, current_buff_len);

    LL_DMA_EnableStream(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE);
    LL_TIM_SetCounter(H_BRIDGE_TIM_HANDLE, 0);
    LL_TIM_EnableCounter(H_BRIDGE_TIM_HANDLE);
}

bool HB_Is_Phase_Done(void) {
    return is_sequence_done;
}

void HB_Pulse_DMA_ISR(void) {
    if (LL_DMA_IsActiveFlag_TC1(H_BRIDGE_DMA_HANDLE)) {
        LL_DMA_ClearFlag_TC1(H_BRIDGE_DMA_HANDLE);

        pulse_cnt--;
        if (pulse_cnt > 0) {
            LL_DMA_DisableStream(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE);
            LL_DMA_SetDataLength(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE, current_buff_len);
            LL_DMA_EnableStream(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE);
        } else {
            LL_TIM_DisableCounter(H_BRIDGE_TIM_HANDLE);
            LL_DMA_DisableStream(H_BRIDGE_DMA_HANDLE, H_BRIDGE_DMA_STREAM_HANDLE);

            HB_Force_Safe_State();

            is_sequence_done = true;
        }
    }
}
