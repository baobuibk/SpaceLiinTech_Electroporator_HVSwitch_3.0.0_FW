/*
 * impedance_task.c
 *
 *  Created on: Jun 25, 2026
 *      Author: PV
 */

#include "impedance_task.h"
#include "app.h"

#include "fsp.h"
#include "fsp_frame.h"
#include "uart_driver.h"
#include "vswitch.h"
#include "h_bridge_driver.h"
#include "board.h"
#include "crc.h"

#include "stm32f4xx_ll_gpio.h"


#include <stdint.h>
#include <stdbool.h>
#include <math.h>


bool                    is_impedance_task_enable = false;
impedance_task_state_t  impedance_task_state = IMPEDANCE_TASK_STATE_IDLE;
uint16_t                impedance_task_volt_charged = 0; 
impedance_pole          impedance_select_pole = {0,0};

uint32_t				im_time_out = 10000;




static void fsp_print(uint8_t packet_length);
static uint16_t impedance_caculate(void);

static uint8_t  voltage_range_count = 0;
static uint16_t impedance_range_count = 0;

static uint16_t voltage_range_array[] =
{
    30,
    50,
    100,
    200,
    300,
};

static uint16_t impedance_range_array[] =
{
    600,
    1000,
    2000,
    4000,
    6000,
};

uint16_t impedance_measure_manual(uint8_t pos_pole, uint8_t neg_pole){

    HB_Set_Pole_Pos_Manual(pos_pole, neg_pole);
    VS_Enable_Hv();
    
    LL_mDelay(50);
    uint16_t impedance_value = impedance_caculate();

    HB_Off();
    VS_Off();  

    return impedance_value;
}


 void impedance_task (void*){

     switch(impedance_task_state){
         case IMPEDANCE_TASK_STATE_IDLE:
         {
             HB_Off();
             VS_Off();

             voltage_range_count = 0;
             impedance_range_count = 0;

             SchedulerTaskDisable(IMPEDANCE_TASK);

             break;
         }
        case IMPEDANCE_TASK_STATE_SET_CHARGE:
        {
            ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_HV;
            ps_FSP_TX -> Payload.set_volt_hv.HV_High =  (uint8_t)((voltage_range_array[voltage_range_count] >> 8) & 0xFF);
            ps_FSP_TX -> Payload.set_volt_hv.HV_Low =   (uint8_t)(voltage_range_array[voltage_range_count] & 0xFF);

            fsp_print(3);

            ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_CONTROL;
            ps_FSP_TX -> Payload.set_charge.HV_cmd_charge = 1;
            ps_FSP_TX -> Payload.set_charge.LV_cmd_charge = 0;
            fsp_print(3);

            im_time_out = 10000;
            impedance_task_state = IMPEDANCE_TASK_STATE_CHARGING;

            ps_FSP_TX -> CMD = FSP_CMD_MEASURE_VOLT;
            fsp_print(1);

            break;
        }
        case IMPEDANCE_TASK_STATE_CHARGING:
        {
 			if(im_time_out > 0) return;

 			im_time_out = 1000;

            float lower = voltage_range_array[voltage_range_count] * (1.0f - 10/100.0f);
            float upper = voltage_range_array[voltage_range_count] * (1.0f + 10/100.0f);

            if(impedance_task_volt_charged < lower || impedance_task_volt_charged > upper)
            {
                ps_FSP_TX -> CMD = FSP_CMD_MEASURE_VOLT;
                fsp_print(1);
            }
            else{
                impedance_task_state = IMPEDANCE_TASK_STATE_INITIAL;
            }
            break;
        }
         case IMPEDANCE_TASK_STATE_INITIAL:
         {
             HB_Set_Pole_Pos_Manual(impedance_select_pole.pos_pole, impedance_select_pole.neg_pole);
             VS_Enable_Hv();
            
             impedance_task_state = IMPEDANCE_TASK_STATE_CACULATE;
             break;
         }
         case IMPEDANCE_TASK_STATE_CACULATE:
         {

             uint16_t impedance_value = impedance_caculate();

             if(impedance_value > impedance_range_array[impedance_range_count]){

                 if(voltage_range_count < (MESURE_IMPEDANCE_RANGE_MAX - 1)){
                     voltage_range_count++;
                     impedance_range_count++;
                    
                     impedance_task_state = IMPEDANCE_TASK_STATE_SET_CHARGE;
                     return;
                 }

             }

             char msg[64];

             if (voltage_range_count >= (MESURE_IMPEDANCE_RANGE_MAX - 1) && impedance_value > impedance_range_array[MESURE_IMPEDANCE_RANGE_MAX - 1]) {
                 sprintf(msg, "IMPEDANCE IS %d Ohm (OVER MAX RANGE)\n\r> ", impedance_value);
             } else {
                 sprintf(msg, "IMPEDANCE IS %d Ohm\n\r> ", impedance_value);
             }
            
             UART_Driver_SendString(&XBEE_UART, msg);

             ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_RELEASE;
             ps_FSP_TX -> Payload.set_discharge.HV_cmd_discharge = 1;
             ps_FSP_TX -> Payload.set_discharge.LV_cmd_discharge = 0;
             fsp_print(3);
     
             impedance_task_state = IMPEDANCE_TASK_STATE_IDLE;
             is_impedance_task_enable = false;

             voltage_range_count = 0;
             impedance_range_count = 0;

             break;
         }
         default:
             break;
     }

     return;
 }


/*-------------------- STATIC FUNCTION-------------------------------*/
static uint16_t impedance_caculate(void){

    float current[10] = {0};
    float volt[10] = {0};

    float    impedance_sum = 0.0;
    uint16_t impedance_average = 0;

    for(uint8_t idx = 0; idx < 10; idx++){

        current[idx] = INA229_GetCurrent(&vom_ina229_dev);
        volt[idx] = INA229_GetBusVoltage(&vom_ina229_dev) * 33.33333333;

        if (current[idx] <= 0) current[idx] = 0.001;

    }

    for (uint8_t idx = 0; idx < 10; idx++)
    {
        impedance_sum += volt[idx] / current[idx];
    }
    
    impedance_average = impedance_sum / 10;

    return impedance_average;
}

static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPC;
	s_FSP_TX_Packet.length 		= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[25] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_Driver_SendFSP(&GPC_UART, (char*)encoded_frame , frame_len);
}
