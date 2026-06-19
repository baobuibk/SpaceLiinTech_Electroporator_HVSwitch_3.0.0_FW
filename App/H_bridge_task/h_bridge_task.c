/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "app.h"
#include "stm32f4xx_ll_gpio.h"

#include "h_bridge_task.h"
#include "h_bridge_driver.h"
#include "v_switch_driver.h"

#include <stdint.h>
#include <stdbool.h>

const Pole_Def_t SYSTEM_POLES[8] = {
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN0_PIN, H_BRIDGE_LIN0_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN1_PIN, H_BRIDGE_LIN1_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN2_PIN, H_BRIDGE_LIN2_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN3_PIN, H_BRIDGE_LIN3_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN4_PIN, H_BRIDGE_LIN4_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN5_PIN, H_BRIDGE_LIN5_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN6_PIN, H_BRIDGE_LIN6_PIN },
    { H_BRIDGE_HANDLE_PORT, H_BRIDGE_HIN7_PIN, H_BRIDGE_LIN7_PIN }
};

bool is_h_bridge_enable = false;

H_Bridge_Sequence_t Sequence_List[MAX_SEQUENCES] = { 0 };
uint8_t total_active_sequences = 0;

H_Bridge_Process_State  H_Bridge_Pulse_Process;
H_Bridge_Task_State     H_Bridge_State;

static uint8_t current_HB_sequence = 0;
static H_Bridge_Sequence_t current_seq;


void H_Bridge_Task_Init(void) {
    is_h_bridge_enable = false;
    V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
    H_Bridge_State = HB_TASK_INIT_STATE;
}

void H_Bridge_Add_Sequence(H_Bridge_Sequence_t new_seq) {
    if (total_active_sequences < MAX_SEQUENCES) {
        Sequence_List[total_active_sequences] = new_seq;
        total_active_sequences++;
    }
}

void H_Bridge_Clear_All_Sequences(void) {
    total_active_sequences = 0;
}


void H_Bridge_Task(void *) {
    if(is_h_bridge_enable == false) {
        V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
        H_Bridge_State = HB_TASK_INIT_STATE;
        current_HB_sequence = 0;
        SchedulerTaskDisable(H_BRIDGE_TASK);
        return;
    }

    switch (H_Bridge_State) {
        case HB_TASK_INIT_STATE: {
            if (total_active_sequences > 0) {
                current_HB_sequence = 0;
                current_seq = Sequence_List[current_HB_sequence];
                H_Bridge_Pulse_Process = HB_HV_POS_PULSE;
                H_Bridge_State = HB_TASK_PULSING;
            }
            break;
        }
        case HB_TASK_PULSING: {

            if (H_Bridge_Pulse_Process == HB_PULSE_IDLE) {
                current_HB_sequence++;

                if (current_HB_sequence >= total_active_sequences) {
                    current_HB_sequence = 0;
                }

                current_seq = Sequence_List[current_HB_sequence];
                H_Bridge_Pulse_Process = HB_HV_POS_PULSE;
            }
            break;
        }
        default:
            break;
    }
}


void H_Bridge_Pulse_Process_Task(void *) {
    if(H_Bridge_State != HB_TASK_PULSING) {
        return;
    }

    HBridge_Config_t hb_cfg;
    uint16_t buff_len = 0;

    switch(H_Bridge_Pulse_Process) {
        case HB_HV_POS_PULSE: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_HV_ON);

                hb_cfg.Port = SYSTEM_POLES[current_seq.pos_pole_index].Port;
                hb_cfg.Pos_HIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].HIN_Pin;
                hb_cfg.Pos_LIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].LIN_Pin;
                hb_cfg.Neg_HIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].HIN_Pin;
                hb_cfg.Neg_LIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].LIN_Pin;
                HB_Init(&hb_cfg);

                buff_len = HB_Build_Pulse_Buff(current_seq.hv_pos_on_ms, current_seq.hv_pos_off_ms);
                HB_Start_DMA_Sequence(buff_len, current_seq.hv_pos_count);

                H_Bridge_Pulse_Process = HB_HV_DELAY;
            }
            break;
        }
        case HB_HV_DELAY: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
                buff_len = HB_Build_Delay_Buff(current_seq.hv_delay_ms);
                HB_Start_DMA_Sequence(buff_len, 1);
                H_Bridge_Pulse_Process = HB_HV_NEG_PULSE;
            }
            break;
        }
        case HB_HV_NEG_PULSE: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_HV_ON);

                hb_cfg.Port = SYSTEM_POLES[current_seq.pos_pole_index].Port;
                hb_cfg.Pos_HIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].HIN_Pin;
                hb_cfg.Pos_LIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].LIN_Pin;
                hb_cfg.Neg_HIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].HIN_Pin;
                hb_cfg.Neg_LIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].LIN_Pin;
                HB_Init(&hb_cfg);

                buff_len = HB_Build_Pulse_Buff(current_seq.hv_neg_on_ms, current_seq.hv_neg_off_ms);
                HB_Start_DMA_Sequence(buff_len, current_seq.hv_neg_count);

                H_Bridge_Pulse_Process = HB_DELAY;
            }
            break;
        }
        case HB_DELAY: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
                buff_len = HB_Build_Delay_Buff(current_seq.pulse_delay_ms);
                HB_Start_DMA_Sequence(buff_len, 1);

                H_Bridge_Pulse_Process = HB_LV_POS_PULSE;
            }
            break;
        }
        case HB_LV_POS_PULSE: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_LV_ON);

                hb_cfg.Port = SYSTEM_POLES[current_seq.pos_pole_index].Port;
                hb_cfg.Pos_HIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].HIN_Pin;
                hb_cfg.Pos_LIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].LIN_Pin;
                hb_cfg.Neg_HIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].HIN_Pin;
                hb_cfg.Neg_LIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].LIN_Pin;
                HB_Init(&hb_cfg);

                buff_len = HB_Build_Pulse_Buff(current_seq.lv_pos_on_ms, current_seq.lv_pos_off_ms);
                HB_Start_DMA_Sequence(buff_len, current_seq.lv_pos_count);

                H_Bridge_Pulse_Process = HB_LV_DELAY;
            }
            break;
        }
        case HB_LV_DELAY: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
                buff_len = HB_Build_Delay_Buff(current_seq.lv_delay_ms);
                HB_Start_DMA_Sequence(buff_len, 1);

                H_Bridge_Pulse_Process = HB_LV_NEG_PULSE;
            }
            break;
        }
        case HB_LV_NEG_PULSE: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_LV_ON);

                hb_cfg.Port = SYSTEM_POLES[current_seq.pos_pole_index].Port;
                hb_cfg.Pos_HIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].HIN_Pin;
                hb_cfg.Pos_LIN_Pin = SYSTEM_POLES[current_seq.neg_pole_index].LIN_Pin;
                hb_cfg.Neg_HIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].HIN_Pin;
                hb_cfg.Neg_LIN_Pin = SYSTEM_POLES[current_seq.pos_pole_index].LIN_Pin;
                HB_Init(&hb_cfg);

                buff_len = HB_Build_Pulse_Buff(current_seq.lv_neg_on_ms, current_seq.lv_neg_off_ms);
                HB_Start_DMA_Sequence(buff_len, current_seq.lv_neg_count);

                H_Bridge_Pulse_Process = HB_SEQUENCE_DELAY;
            }
            break;
        }
        case HB_SEQUENCE_DELAY: {
            if(HB_Is_Phase_Done() == true) {
                V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
                buff_len = HB_Build_Delay_Buff(current_seq.sequence_delay_ms);
                HB_Start_DMA_Sequence(buff_len, 1);

                H_Bridge_Pulse_Process = HB_PULSE_IDLE;
            }
            break;
        }
        case HB_PULSE_IDLE: {

            break;
        }
        default:
            break;
    }
}
