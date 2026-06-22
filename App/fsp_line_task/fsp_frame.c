/*
 * fsp_frame.c
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */
#include "fsp_frame.h"
#include "fsp_line_task.h"
#include "cli_command.h"


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "app.h"







static void fsp_print(uint8_t packet_length);


uint8_t FSP_Line_Process(void)
{
	switch(ps_FSP_RX -> CMD)
	{
	case FSP_CMD_GET_CAP_CONTROL:
	{
		hv_is_charging = ps_FSP_RX -> Payload.get_cap_control.HV_Volt_Charge;
		lv_is_charging = ps_FSP_RX -> Payload.get_cap_control.LV_Volt_Charge;
	}
	case FSP_CMD_GET_CAP_FINISH_CHARGE:
	{
		bool hv_finish_charge = ps_FSP_RX -> Payload.finish_charge_flag.HV_charge_finish_flag;
		bool lv_finish_charge = ps_FSP_RX -> Payload.finish_charge_flag.LV_charge_finish_flag;

		if(hv_finish_charge == true){
			char msg[64];

			sprintf(msg,"HV CAP FINISHED CHARGING TO %dV\n\r", hv_set_volt);
			UART_Driver_SendString(CMD_line_handle,msg);
		}
		if(lv_finish_charge == true){
			char msg[64];

			sprintf(msg,"LV CAP FINISHED CHARGING TO %dV\n\r", lv_set_volt);
			UART_Driver_SendString(CMD_line_handle,msg);
		}
	}
	case FSP_CMD_GET_CAP_FINISH_DISCHARGE:
	{
		bool hv_finish_discharge = ps_FSP_RX -> Payload.finish_discharge_flag.HV_discharge_finish_flag;
		bool lv_finish_discharge = ps_FSP_RX -> Payload.finish_discharge_flag.LV_discharge_finish_flag;

		if(hv_finish_discharge == true){
			char msg[64];

			sprintf(msg,"HV CAP FINISHED CHARGING TO %dV\n\r", hv_set_volt);
			UART_Driver_SendString(CMD_line_handle,msg);
		}
		if(lv_finish_discharge == true){
			char msg[64];

			sprintf(msg,"LV CAP FINISHED CHARGING TO %dV\n\r", lv_set_volt);
			UART_Driver_SendString(CMD_line_handle,msg);
		}
	}
	case FSP_CMD_MEASURE_VOLT:
	{
		hv_raw_volt = 	((uint16_t)(ps_FSP_RX -> Payload.measure_volt.HV_raw_volt_high)<<8)
						|(ps_FSP_RX -> Payload.measure_volt.HV_raw_volt_low);
		lv_raw_volt =	 ((uint16_t)(ps_FSP_RX -> Payload.measure_volt.LV_raw_volt_high)<<8)
						|(ps_FSP_RX -> Payload.measure_volt.LV_raw_volt_low);
	}
	default:
		return 0;

	}
}


static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
	s_FSP_TX_Packet.length 		= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[25] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_Driver_SendFSP(&GPP_UART, (char*)encoded_frame , frame_len);
}

