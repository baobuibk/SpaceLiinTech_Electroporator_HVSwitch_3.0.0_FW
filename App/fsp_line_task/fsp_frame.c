/*
 * fsp_frame.c
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */
#include "fsp.h"
#include "fsp_frame.h"
#include "fsp_line_task.h"
#include "cli_command.h"
#include "impedance_task.h"


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
	/*---------- Handle response control CMD--------------*/
	case FSP_CMD_SET_CAP_VOLT_ALL:
		break;

	case FSP_CMD_SET_CAP_VOLT_HV:
		break;

	case FSP_CMD_SET_CAP_VOLT_LV:
		break;

	case FSP_CMD_SET_CAP_CONTROL:
		break;

	case FSP_CMD_SET_CAP_RELEASE:
		break;

	/*---------- Response information user request--------------*/
	case FSP_CMD_GET_CAP_STATE:
	{
		if (ps_FSP_RX->Payload.get_cap_state.hv_state_idle == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS IDLE");

		else if (ps_FSP_RX->Payload.get_cap_state.hv_state_charging == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS CHARGING");

		else if (ps_FSP_RX->Payload.get_cap_state.hv_state_discharging == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS DISCHARGING");

		else if (ps_FSP_RX->Payload.get_cap_state.hv_state_holding_volt == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS HOLDING VOLT");



		if (ps_FSP_RX->Payload.get_cap_state.lv_state_idle == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS IDLE");

		else if (ps_FSP_RX->Payload.get_cap_state.lv_state_charging == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS CHARGING");

		else if (ps_FSP_RX->Payload.get_cap_state.lv_state_discharging == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS DISCHARGING");

		else if (ps_FSP_RX->Payload.get_cap_state.lv_state_holding_volt == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS HOLDING VOLT");

		break;
	}
	case FSP_CMD_GET_CAP_VOLT:
	{
		char msg[64];

		uint16_t hv_set_vol = 	((uint16_t)ps_FSP_RX -> Payload.get_cap_volt.HV_Volt_High << 8)
								| ps_FSP_RX -> Payload.get_cap_volt.HV_Volt_Low;

		uint16_t lv_set_vol = 	((uint16_t)ps_FSP_RX -> Payload.get_cap_volt.LV_Volt_High << 8)
								| ps_FSP_RX -> Payload.get_cap_volt.LV_Volt_Low;

		sprintf(msg,"\r\n> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n", hv_set_vol, lv_set_vol);

		UART_Driver_SendString(&XBEE_UART, msg);

		break;
	}
	case FSP_CMD_GET_CAP_CONTROL:
	{
		if(ps_FSP_RX -> Payload.get_cap_control.HV_Volt_Charge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS CHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_control.HV_Volt_Charge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS NOT CHARGING");

		if(ps_FSP_RX -> Payload.get_cap_control.LV_Volt_Charge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS CHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_control.LV_Volt_Charge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS NOT CHARGING");

		break;
	}
	case FSP_CMD_GET_CAP_RELEASE:
	{
		if(ps_FSP_RX -> Payload.get_cap_release.HV_Volt_Discharge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS DISCHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_release.HV_Volt_Discharge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS NOT DISCHARGING");

		if(ps_FSP_RX -> Payload.get_cap_release.LV_Volt_Discharge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS DISCHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_release.LV_Volt_Discharge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS NOT DISCHARGING");
		break;
	}
	case FSP_CMD_GET_CAP_ALL:
	{
		char msg[64];

		uint16_t hv_set_vol = 	((uint16_t)ps_FSP_RX -> Payload.get_cap_all.HV_Volt_High << 8)
								| ps_FSP_RX -> Payload.get_cap_volt.HV_Volt_Low;

		uint16_t lv_set_vol = 	((uint16_t)ps_FSP_RX -> Payload.get_cap_all.LV_Volt_High << 8)
								| ps_FSP_RX -> Payload.get_cap_volt.LV_Volt_Low;

		sprintf(msg,"\r\n> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n", hv_set_vol, lv_set_vol);

		UART_Driver_SendString(&XBEE_UART, msg);



		if(ps_FSP_RX -> Payload.get_cap_all.HV_Volt_Charge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS CHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_all.HV_Volt_Charge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS NOT CHARGING");

		if(ps_FSP_RX -> Payload.get_cap_all.LV_Volt_Charge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS CHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_all.LV_Volt_Charge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS NOT CHARGING");



		if(ps_FSP_RX -> Payload.get_cap_all.HV_Volt_Discharge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS DISCHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_all.HV_Volt_Discharge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nHV CAP IS NOT DISCHARGING");

		if(ps_FSP_RX -> Payload.get_cap_all.LV_Volt_Discharge == true)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS DISCHARGING");

		else if(ps_FSP_RX -> Payload.get_cap_all.LV_Volt_Discharge == false)
			UART_Driver_SendString(&XBEE_UART,"\r\nLV CAP IS NOT DISCHARGING");

		break;
	}
	case FSP_CMD_MEASURE_VOLT:
	{
		char msg[64];

		uint16_t hv_raw_vol = 	((uint16_t)ps_FSP_RX -> Payload.measure_volt.HV_raw_volt_high << 8)
								| ps_FSP_RX -> Payload.measure_volt.HV_raw_volt_low;

		uint16_t lv_raw_vol = 	((uint16_t)ps_FSP_RX -> Payload.measure_volt.LV_raw_volt_high << 8)
								| ps_FSP_RX -> Payload.measure_volt.LV_raw_volt_low;

		if(is_measure_volt_notify_enable == true){
			sprintf(msg, "HV cap: %dV, LV cap: %dV\n\r> ", hv_raw_vol, lv_raw_vol);
			UART_Driver_SendString(&XBEE_UART, msg);

			is_measure_volt_notify_enable = false;
		}

		if(impedance_task_state == IMPEDANCE_TASK_STATE_CHARGING)
		{
			impedance_task_volt_charged = hv_raw_vol;
		}

		break;
	}
	case FSP_CMD_GET_OVV_FLAG:
	{
		char msg[64];
		bool HV_OVV_Flag = ps_FSP_RX -> Payload.get_ovv_flag.HV_OVV_flag;
		bool LV_OVV_Flag = ps_FSP_RX -> Payload.get_ovv_flag.LV_OVV_flag;

		if(HV_OVV_Flag == true){
			UART_Driver_SendString(&XBEE_UART, "HV CAP IS OVER VOLTAGE\r\n> ");
		}
		else if(HV_OVV_Flag == false){
			UART_Driver_SendString(&XBEE_UART, "HV CAP VOLTAGE OK\r\n> ");
		}

		if(LV_OVV_Flag == true){
			UART_Driver_SendString(&XBEE_UART, "LV CAP IS OVER VOLTAGE\r\n> ");
		}
		else if(LV_OVV_Flag == false){
			UART_Driver_SendString(&XBEE_UART, "LV CAP VOLTAGE OK\r\n> ");
		}
		break;

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

	UART_Driver_SendFSP(&GPC_UART, (char*)encoded_frame , frame_len);
}

