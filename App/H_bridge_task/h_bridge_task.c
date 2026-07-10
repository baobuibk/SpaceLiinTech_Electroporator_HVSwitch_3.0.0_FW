/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "app.h"
#include "stm32f4xx_ll_gpio.h"

#include "h_bridge_task.h"
#include "h_bridge_driver.h"
#include "vswitch.h"
#include "xbee_cmd_task.h"

#include <stdint.h>
#include <stdbool.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Global Variable~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

H_Bridge_Sequence_t     Sequence_List[MAX_SEQUENCES] = { 0 };
uint8_t                 total_active_sequences = 0;
H_Bridge_Task_State     H_Bridge_State;
H_Bridge_Task_Mode      H_Bridge_Mode;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variable~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static H_Bridge_Sequence_t default_sequence = {
    .is_edit_enable = false,
	.is_confirm = false,

    .sequence_delay_ms = 100,

    .pos_pole_index = 1,
    .neg_pole_index = 6,

    .hv_pos_count = 20,
    .hv_neg_count = 20,

    .hv_delay_ms = 100,

    .hv_pos_on_ms = 100,
    .hv_pos_off_ms = 1000,
    .hv_neg_on_ms = 100,
    .hv_neg_off_ms = 1000,

    .pulse_delay_ms = 1000,

    .lv_pos_count = 20,
    .lv_neg_count = 20,

    .lv_delay_ms = 100,

    .lv_pos_on_ms = 1000,
    .lv_pos_off_ms = 1000,
    .lv_neg_on_ms = 1000,
    .lv_neg_off_ms = 1000,
};

static H_Bridge_Sequence_t current_seq;


void H_Bridge_Task_Init(void) {

    HB_Off();
    VS_Off();

    total_active_sequences = 0;

    H_Bridge_State = HB_TASK_IDLE;
    H_Bridge_Mode = HB_MODE_MANUAL;

    for (uint8_t i = 0; i < MAX_SEQUENCES; i++)
    {
        Sequence_List[i] = default_sequence;
    }
}

H_Bridge_Task_State H_Bridge_Task_Get_State(void) {
    return H_Bridge_State;
}

void H_Bridge_Task_Set_State(H_Bridge_Task_State state) {
    H_Bridge_State = state;
}


/*
Currently, DMA buffer count is not calculated for maximum usage
Currnetly it is set at 3200
Recalculated if needed!

NOTE: DEADTIME currently having bug that delay for 200us instead of 100us!!!!
NOTE: Timer period is 100us

Ton and Tof must * 10 (1ms = 10 * 100us)
Deadtime = 1 * 100us
(Ton * 10) - 1: Ton in SALEA always Ton + 100us
 HB_Pulse to add one type of pulse
 HB Delay: ch1 and ch2 must be the one currently in use

*/

void H_Bridge_Task(void *) {

    switch (H_Bridge_State) {
        case HB_TASK_IDLE: 
        {
			total_active_sequences = 0;

             if(H_Bridge_Mode == HB_MODE_AUTO_ACCEL)
            	 H_Bridge_Mode = HB_MODE_MANUAL;

			SchedulerTaskDisable(H_BRIDGE_TASK);
			break;
        }

        case HB_TASK_INIT_STATE: 
        {
            if (total_active_sequences <= 0) {


                break;
            }
            // Calculate BSRR for the first sequence
            uint8_t current_HB_sequence_index = 0;
            VS_Clear_Sequence();
            HB_Clear_Sequence();

            for (current_HB_sequence_index = 0; current_HB_sequence_index < total_active_sequences; current_HB_sequence_index++)
            {
                current_seq = Sequence_List[current_HB_sequence_index];
                uint32_t vs_hv_ontime_us = 0;
                uint32_t vs_lv_ontime_us = 0;
                uint32_t vs_deadtime_us = 1;

                // Sequence delay
                vs_hv_ontime_us += HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.sequence_delay_ms * 10) - 1);
                // HV Pos
                vs_hv_ontime_us += HB_Pulse(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.hv_pos_on_ms * 10) - 1, (current_seq.hv_pos_off_ms * 10) - 1, 1, current_seq.hv_pos_count);
                // HV delay
                vs_hv_ontime_us += HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.hv_delay_ms * 10) - 1);
                // HV Neg
                vs_hv_ontime_us += HB_Pulse(current_seq.neg_pole_index, current_seq.pos_pole_index, (current_seq.hv_neg_on_ms * 10) - 1, (current_seq.hv_neg_off_ms * 10) - 1, 1, current_seq.hv_neg_count);
                vs_hv_ontime_us -= 1;
                // Pulse delay
                vs_lv_ontime_us += HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.pulse_delay_ms * 10) - 1);
                // LV Pos
                vs_lv_ontime_us += HB_Pulse(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.lv_pos_on_ms * 10) - 1, (current_seq.lv_pos_off_ms * 10) - 1, 1, current_seq.lv_pos_count);
                // LV delay
                vs_lv_ontime_us += HB_Delay(current_seq.pos_pole_index, current_seq.neg_pole_index, (current_seq.lv_delay_ms * 10) - 1);
                // LV Neg
                vs_lv_ontime_us += HB_Pulse(current_seq.neg_pole_index, current_seq.pos_pole_index, (current_seq.lv_neg_on_ms * 10) - 1, (current_seq.lv_neg_off_ms * 10) - 1, 1, current_seq.lv_neg_count);
                vs_lv_ontime_us -= 1;  

                VS_Pulse(vs_hv_ontime_us, vs_lv_ontime_us, vs_deadtime_us, current_HB_sequence_index);
            }

            VS_Start();
            HB_Start();
            VOM_Sampling_Start();
            
            H_Bridge_State = HB_TASK_PULSING;

            break;
        }
        case HB_TASK_PULSING: 
        {
            if (HB_Is_Phase_Done() == true)
            {
            	LL_TIM_DisableCounter(TIM1);

                HB_Off();
                VS_Off();
                VOM_Sampling_Stop();

                UART_Driver_SendString(&XBEE_UART,"H BRIDGE PULSING DONE\r\n> ");
                H_Bridge_State = HB_TASK_IDLE;
            }
            break;
        }

        case HB_TASK_ERROR_STATE:
        {
            HB_Stop_Priority();
            VS_Stop_Priority();

            HB_Off();
            VS_Off();
            VOM_Sampling_Stop();

            H_Bridge_State = HB_TASK_IDLE;
    
            break;
        }
        default:
            break;
    }

    return;
}
