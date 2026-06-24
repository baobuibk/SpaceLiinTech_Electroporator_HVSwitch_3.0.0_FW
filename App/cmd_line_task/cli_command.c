/*
 * cli_command.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#include "cli/CLI_Src/embedded_cli.h"
#include "cli/CLI_Setup/cli_setup.h"
#include "cli_command.h"

#include "fsp_line_task.h"
#include "fsp_frame.h"
#include "fsp.h"

#include "uart_driver.h"
#include "board.h"
#include "app.h"

#include <stdlib.h>
#include <string.h>






/*************************************************
 *                Private variable                 *
 *************************************************/
static uint8_t ChannelMapping[8] = {0, 1, 2, 3, 4, 5, 6, 7};


/*************************************************
 *                GLobal variable                 *
 *************************************************/
uint8_t CMD_sequence_index = 0;

/*************************************************
 *                Command Define                 *
 *************************************************/
static void fsp_print(uint8_t packet_length);

static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);

/*----------------------CMD FOR CAP CONTROL--------------------------------*/

static void	CMD_SET_CAP_VOLT_ALL(EmbeddedCli *cli, char *args, void *context);
static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_CAP_STATE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_CAP_VOLT (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context);
static void	CMD_GET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_CAP_ALL (EmbeddedCli *cli, char *args, void *context);

static void CMD_MEASURE_VOLT (EmbeddedCli *cli, char *args, void *context);

/*----------------------CMD FOR H BRIDGE CONTROL-----------------------------*/

static void	CMD_SET_SEQUENCE_INDEX(EmbeddedCli *cli, char *args, void *context);
static void	CMD_SET_SEQUENCE_DELETE (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_SEQUENCE_CONFIRM (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_SEQUENCE_DELAY (EmbeddedCli *cli, char *args, void *context);

static void CMD_SET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_COUNT (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_DELAY (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_HV_POS (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_HV_NEG (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_LV_POS (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_LV_NEG (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_PULSE_CONTROL (EmbeddedCli *cli, char *args, void *context);

/*************************************************
 *                 Command  Array                *
 *************************************************/


static const CliCommandBinding cliStaticBindings_internal[] = {
    { NULL, "HELP",         "format: help",                                         false, NULL, CMD_Help },
    { NULL, "RESET",        "Reset MCU: reset",                                     false, NULL, CMD_Reset },
    { NULL, "CLR",          "Clears the console",                                   false, NULL, CMD_ClearCLI },

	{ NULL,	"SET_CAP_VOLT_ALL", 		"format: SET_CAP_VOLT_ALL [HV_Volt] [LV_Volt]",		true,NULL,CMD_SET_CAP_VOLT_ALL},
	{ NULL,	"SET_CAP_VOLT_HV", 			"format: SET_CAP_VOLT_HV [HV_Volt]",				true,NULL,CMD_SET_CAP_VOLT_HV},
	{ NULL,	"SET_CAP_VOLT_LV", 			"format: SET_CAP_VOLT_LV [LV_Volt]",				true,NULL,CMD_SET_CAP_VOLT_LV},
	{ NULL,	"SET_CAP_CONTROL", 			"format: SET_CAP_CONTROL [N] [S]",					true,NULL,CMD_SET_CAP_CONTROL},
	{ NULL,	"SET_CAP_RELEASE", 			"format: SET_CAP_RELEASE [N] [S]",					true,NULL,CMD_SET_CAP_RELEASE},

	{ NULL,	"GET_CAP_VOLT", 			"format: GET_CAP_VOLT ",							false,NULL,CMD_GET_CAP_VOLT},
	{ NULL,	"GET_CAP_RELEASE", 			"format: GET_CAP_RELEASE",							false,NULL,CMD_GET_CAP_RELEASE},
	{ NULL,	"GET_CAP_CONTROL", 			"format: GET_CAP_CONTROL",							false,NULL,CMD_GET_CAP_CONTROL},
	{ NULL,	"GET_CAP_ALL", 				"format: GET_CAP_ALL",								false,NULL,CMD_GET_CAP_ALL},
	{ NULL,	"GET_CAP_STATE", 			"format: GET_CAP_STATE",							false,NULL,CMD_GET_CAP_STATE},

	{ NULL,	"MEASURE_VOLT", 			"format: MEASURE_VOLT",							false,NULL,CMD_MEASURE_VOLT},

	{ NULL,	"SET_SEQUENCE_INDEX", 		"format: SET_SEQUENCE_INDEX [N]",				true,	NULL,CMD_SET_SEQUENCE_INDEX},
	{ NULL,	"SET_SEQUENCE_DELETE", 		"format: SET_SEQUENCE_DELETE",					false,	NULL,CMD_SET_SEQUENCE_DELETE},
	{ NULL,	"SET_SEQUENCE_CONFIRM", 	"format: SET_SEQUENCE_CONFIRM",					false,	NULL,CMD_SET_SEQUENCE_CONFIRM},
	{ NULL,	"SET_SEQUENCE_DELAY", 		"format: SET_SEQUENCE_DELAY [N]",				true,	NULL,CMD_SET_SEQUENCE_DELAY},

	{ NULL,	"SET_PULSE_POLE", 			"format: SET_PULSE_POLE [N] [M]",				true,	NULL,CMD_SET_PULSE_POLE},
	{ NULL,	"SET_PULSE_COUNT", 			"format: SET_PULSE_COUNT [N] [S] [X] [Y]",		true,	NULL,CMD_SET_PULSE_COUNT},
	{ NULL,	"SET_PULSE_DELAY", 			"format: SET_PULSE_DELAY [N] [M] [X]",			true,	NULL,CMD_SET_PULSE_DELAY},
	{ NULL,	"SET_PULSE_HV_POS", 		"format: SET_PULSE_HV_POS [N] [S]",				true,	NULL,CMD_SET_PULSE_HV_POS},
	{ NULL,	"SET_PULSE_HV_NEG", 		"format: SET_PULSE_HV_NEG [N] [S]",				true,	NULL,CMD_SET_PULSE_HV_NEG},
	{ NULL,	"SET_PULSE_LV_POS", 		"format: SET_PULSE_LV_POS [N] [S]",				true,	NULL,CMD_SET_PULSE_LV_POS},
	{ NULL,	"SET_PULSE_LV_NEG", 		"format: SET_PULSE_LV_NEG [N] [S]",				true,	NULL,CMD_SET_PULSE_LV_NEG},
	{ NULL,	"SET_PULSE_CONTROL", 		"format: SET_PULSE_CONTROL [N]",				true,	NULL,CMD_SET_PULSE_CONTROL},
};

/*************************************************
 *             Command List Function             *
 *************************************************/

/*----------------------CMD FOR CAP CONTROL--------------------------------*/

static void	CMD_SET_CAP_VOLT_ALL(EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);

	if(argc < 2) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	uint16_t receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 300) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 50) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_ALL;
	ps_FSP_TX -> Payload.set_volt_all.HV_High = (uint8_t) ((receive_argm[0]>>8) & 0xFF);
	ps_FSP_TX -> Payload.set_volt_all.HV_Low = (uint8_t) (receive_argm[0] & 0xFF);
	ps_FSP_TX -> Payload.set_volt_all.LV_High = (uint8_t) ((receive_argm[1]>>8) & 0xFF);;
	ps_FSP_TX -> Payload.set_volt_all.LV_Low = (uint8_t) (receive_argm[1] & 0xFF);

	fsp_print(5);

	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
	return;
}

static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc < 1) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 1){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t receive_argm = atoi(embeddedCliGetToken(args, 1));

	if ((receive_argm > 300) || (receive_argm < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_HV;
	ps_FSP_TX -> Payload.set_volt_hv.HV_High = (uint8_t) ((receive_argm>>8) & 0xFF);
	ps_FSP_TX -> Payload.set_volt_hv.HV_Low = (uint8_t) (receive_argm & 0xFF);

	fsp_print(3);

	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");

	return;
}

static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc < 1) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 1){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t receive_argm = atoi(embeddedCliGetToken(args, 1));

	if ((receive_argm > 50) || (receive_argm < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_LV;
	ps_FSP_TX -> Payload.set_volt_lv.LV_High = (uint8_t) ((receive_argm>>8) & 0xFF);
	ps_FSP_TX -> Payload.set_volt_lv.LV_Low = (uint8_t) (receive_argm & 0xFF);

	fsp_print(3);

	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");

	return;
}

static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc < 2) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_CONTROL;
	ps_FSP_TX -> Payload.set_charge.HV_cmd_charge = receive_argm[0];
	ps_FSP_TX -> Payload.set_charge.LV_cmd_charge = receive_argm[1];

	fsp_print(3);

	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");

	return ;
}

static void CMD_SET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);

	if(argc < 2) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_RELEASE;
	ps_FSP_TX -> Payload.set_discharge.HV_cmd_discharge = receive_argm[0];
	ps_FSP_TX -> Payload.set_discharge.LV_cmd_discharge = receive_argm[1];

	fsp_print(3);
	embeddedCliPrint(cli,"CMDLINE_OK\n\r");
	return ;

}


static void CMD_GET_CAP_VOLT (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_VOLT;
	fsp_print(1);

	embeddedCliPrint(cli,"CMDLINE_OK\n\r");

	return;         
}

static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_CONTROL;
	fsp_print(1);
	
	embeddedCliPrint(cli,"CMDLINE_OK\n\r");

	return;
}

static void	CMD_GET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_RELEASE;
	fsp_print(1);
	
	embeddedCliPrint(cli,"CMDLINE_OK\n\r");

	return;
}

static void CMD_GET_CAP_ALL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_ALL;
	fsp_print(1);
	
	embeddedCliPrint(cli,"CMDLINE_OK\n\r");

	return;
}

static void CMD_GET_CAP_STATE (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_STATE;
	fsp_print(1);

	embeddedCliPrint(cli,"CMDLINE_OK\n\r");

	return;
}


static void CMD_MEASURE_VOLT (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_MEASURE_VOLT;
	fsp_print(1);
	
	embeddedCliPrint(cli,"CMDLINE_OK\n\r");

	return;
}


/*----------------------CMD FOR H BRIDGE CONTROL-----------------------------*/
static void	CMD_SET_SEQUENCE_INDEX(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}

	else if (argc > 1) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	uint8_t user_target_seq_idx;

	user_target_seq_idx = atoi(embeddedCliGetToken(args, 1));

	if (user_target_seq_idx < 1 || user_target_seq_idx > MAX_SEQUENCES) {
		char msg [64];
		return;
	}

	if(user_target_seq_idx > (CMD_sequence_index + 2)){
		return;
	}

	CMD_sequence_index = user_target_seq_idx - 1;

	Sequence_List[CMD_sequence_index].is_edit_enable = true;

	total_active_sequences = CMD_sequence_index + 1;
	return;
}

static void	CMD_SET_SEQUENCE_DELETE(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
	}

	Sequence_List[CMD_sequence_index].is_edit_enable = false;

	total_active_sequences = CMD_sequence_index--;

}

static void	CMD_SET_SEQUENCE_CONFIRM(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	Sequence_List[CMD_sequence_index].is_edit_enable = false;
	Sequence_List[CMD_sequence_index].is_confirm	 = true;

}

static void	CMD_SET_SEQUENCE_DELAY(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 1) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	if (Sequence_List[CMD_sequence_index].is_edit_enable == true) {
		uint8_t receive_argm = atoi(embeddedCliGetToken(args, 1));
		Sequence_List[CMD_sequence_index].sequence_delay_ms = receive_argm;
	}
	return;
}


static void	CMD_SET_PULSE_POLE(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	uint8_t receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].pos_pole_index =  ChannelMapping[receive_argm[0] - 1];
	Sequence_List[CMD_sequence_index].neg_pole_index =  ChannelMapping[receive_argm[1] - 1];

	return;
}

static void	CMD_SET_PULSE_COUNT(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 4) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 4) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[4];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
	receive_argm[2] = atoi(embeddedCliGetToken(args, 3));
	receive_argm[3] = atoi(embeddedCliGetToken(args, 4));

	if ((receive_argm[0] > 20) || (receive_argm[1] > 20) || (receive_argm[2] > 20) || (receive_argm[3] > 20)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].hv_pos_count 	= receive_argm[0];
	Sequence_List[CMD_sequence_index].hv_neg_count 	= receive_argm[1];

    Sequence_List[CMD_sequence_index].lv_pos_count 	= receive_argm[2];
    Sequence_List[CMD_sequence_index].lv_neg_count 	= receive_argm[3];

    return;
}

static void	CMD_SET_PULSE_DELAY(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 3) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 3) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[3];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
	receive_argm[2] = atoi(embeddedCliGetToken(args, 3));

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].hv_delay_ms = receive_argm[0];
	Sequence_List[CMD_sequence_index].lv_delay_ms = receive_argm[1];
	Sequence_List[CMD_sequence_index].pulse_delay_ms = receive_argm[2];

	embeddedCliPrint(cli, "\n\r> CMDLINE_OK");

	return;
}

static void	CMD_SET_PULSE_HV_POS(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].hv_pos_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].hv_pos_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "\n\r> CMDLINE_OK");

	return;
}

static void	CMD_SET_PULSE_HV_NEG(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].hv_neg_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].hv_neg_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "\n\r> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_LV_POS(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].lv_pos_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].lv_pos_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "\n\r> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_LV_NEG(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].lv_neg_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].lv_neg_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "\n\r> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_CONTROL(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 1) {
		embeddedCliPrint(cli, "\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	uint8_t receive_argm;

	receive_argm = atoi(embeddedCliGetToken(args, 1));

	if (receive_argm != 1)
		return;

	is_h_bridge_enable = receive_argm;
	SchedulerTaskEnable(H_BRIDGE_TASK, 1);

	return;
}







/*************************************************
 *                  End CMD List                 *
 *************************************************/


/*************************************************
 *                Getter - Helper                *
 *************************************************/
static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "\33[2J");
    embeddedCliPrint(cli, buffer);
}

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context) {
	NVIC_SystemReset();
    embeddedCliPrint(cli, "");
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

const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
