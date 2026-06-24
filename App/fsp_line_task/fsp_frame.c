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
	case FSP_CMD_GET_CAP_CONTROL:
		break;

	case FSP_CMD_GET_CAP_RELEASE:
		break;

	case FSP_CMD_GET_CAP_VOLT:
		break;

	case FSP_CMD_GET_CAP_ALL:
		break;

	case FSP_CMD_GET_CAP_FINISH_CHARGE:
		break;

	case FSP_CMD_GET_CAP_FINISH_DISCHARGE:
		break;

	case FSP_CMD_GET_CAP_IS_CHARGING:
		break;

	case FSP_CMD_MEASURE_VOLT:
		break;

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

