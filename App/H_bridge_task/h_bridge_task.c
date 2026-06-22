/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "app.h"
#include "stm32f4xx_ll_gpio.h"

#include "h_bridge_task.h"

#include "h_bridge_driver.h"
#include "v_switch_driver.h"

#include <stdint.h>
#include <stdbool.h>

int haha = 0;

bool is_h_bridge_enable = false;

H_Bridge_Sequence_t Sequence_List[MAX_SEQUENCES] = { 0 };
uint8_t total_active_sequences = 0;


H_Bridge_Task_State     H_Bridge_State;

static uint8_t current_HB_sequence_index = 0;

static H_Bridge_Sequence_t default_sequence = {
    .is_edit_enable = false,
	.is_confirm = false,

    .sequence_delay_ms = 5,

    .pos_pole_index = 0,
    .neg_pole_index = 1,

    .pulse_delay_ms = 15,

    .hv_pos_count = 1,
    .hv_neg_count = 1,

    .hv_delay_ms = 5,

    .hv_pos_on_ms = 10,
    .hv_pos_off_ms = 10,
    .hv_neg_on_ms = 10,
    .hv_neg_off_ms = 10,

    .lv_pos_count = 20,
    .lv_neg_count = 20,

    .lv_delay_ms = 10,

    .lv_pos_on_ms = 1000,
    .lv_pos_off_ms = 1000,
    .lv_neg_on_ms = 1000,
    .lv_neg_off_ms = 1000,
};

static H_Bridge_Sequence_t current_seq;


void H_Bridge_Task_Init(void) {

    is_h_bridge_enable = false;

    V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
    H_Bridge_State = HB_TASK_INIT_STATE;


    for (uint8_t i = 0; i < MAX_SEQUENCES; i++)
    {
        Sequence_List[i] = default_sequence;
    }
}



void H_Bridge_Task(void *) {

    if(is_h_bridge_enable == false)
    {
        V_Switch_Set_Mode(V_SWITCH_MODE_ALL_OFF);
        H_Bridge_State = HB_TASK_INIT_STATE;
        current_HB_sequence_index = 0;

        SchedulerTaskDisable(H_BRIDGE_TASK);

        return;
    }

    switch (H_Bridge_State) {
        case HB_TASK_INIT_STATE: {
            if (total_active_sequences <= 0) {
                break;
            }

            // Calculate BSRR for the first sequence
            current_HB_sequence_index = 0;

            // Currently, DMA buffer count is not calculated for maximum usage
            // Currnetly it is set at 3200
            // Recalculated if needed!

            // NOTE: DEADTIME currently having bug that delay for 200us instead of 100us!!!!
            for (current_HB_sequence_index = 0; current_HB_sequence_index < total_active_sequences; current_HB_sequence_index++)
            {
                current_seq = Sequence_List[current_HB_sequence_index];
                // NOTE: Timer period is 100us
                // Ton and Tof must * 10 (1ms = 10 * 100us)
                // Deadtime = 1 * 100us
                // (Ton * 10) - 1: Ton in SALEA always Ton + 100us
                // HB_Pulse to add one type of pulse

                // HB Delay: ch1 and ch2 must be the one currently in use

                // Sequence delay
                HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.sequence_delay_ms * 10) - 1);

                // HV Pos
                HB_Pulse(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.hv_pos_on_ms * 10) - 1, (current_seq.hv_pos_off_ms * 10) - 1, 1, current_seq.hv_pos_count);
                // HV delay
                HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.hv_delay_ms * 10) - 1);
                // HV Neg
                HB_Pulse(current_seq.neg_pole_index, current_seq.pos_pole_index, (current_seq.hv_neg_on_ms * 10) - 1, (current_seq.hv_neg_off_ms * 10) - 1, 1, current_seq.hv_neg_count);
                
                // Pulse delay
                HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.pulse_delay_ms * 10) - 1);

                // LV Pos
                HB_Pulse(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.lv_pos_on_ms * 10) - 1, (current_seq.lv_pos_off_ms * 10) - 1, 1, current_seq.lv_pos_count);
                // LV delay
                HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.lv_delay_ms * 10) - 1);
                // LV Neg
                HB_Pulse(current_seq.neg_pole_index, current_seq.pos_pole_index, (current_seq.lv_neg_on_ms * 10) - 1, (current_seq.lv_neg_off_ms * 10) - 1, 1, current_seq.lv_neg_count);

                uint32_t hv_on_us =(current_seq.sequence_delay_ms * 10)
                					+ (((current_seq.hv_pos_on_ms * 10)  + (current_seq.hv_pos_off_ms * 10)  + 1) * current_seq.hv_pos_count)
									+ (current_seq.hv_delay_ms * 10)
									+ (((current_seq.hv_neg_on_ms * 10) + (current_seq.hv_neg_off_ms * 10)  + 1) * current_seq.hv_neg_count);


                uint32_t lv_on_us = (((current_seq.lv_pos_on_ms * 10)  + (current_seq.lv_pos_off_ms * 10)  + 1) * current_seq.lv_pos_count)
									+ (current_seq.lv_delay_ms * 10)
									+ (((current_seq.lv_neg_on_ms * 10)  + (current_seq.lv_neg_off_ms * 10) + 1) * current_seq.lv_neg_count)
                					+ (current_seq.pulse_delay_ms * 10);
                uint32_t dead_us = 1;


                V_Switch_Change_Phase_Calc(hv_on_us, lv_on_us, dead_us, current_HB_sequence_index);


            }
            HB_Start();

            V_Switch_Start();



            
            H_Bridge_State = HB_TASK_PULSING;

            break;
        }
        case HB_TASK_PULSING: {
            if (HB_Is_Phase_Done() == true)
            {
                H_Bridge_State = HB_TASK_INIT_STATE;
                is_h_bridge_enable = false;
                V_Switch_Clear_BuffARR();

                break;
            }
        }
        default:
            break;
    }
}
