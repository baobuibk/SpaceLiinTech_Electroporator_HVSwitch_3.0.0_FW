/*
 * cli_command.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#include <cli/CLI_Setup/cli_setup.h>
#include <cli/CLI_Src/embedded_cli.h>
#include "cli_command.h"

#include "board.h"
#include "app.h"

#include <stdlib.h>
#include <string.h>


/*************************************************
 *                Private variable                 *
 *************************************************/
//static uint8_t CMD_process_state = 0;
//static uint16_t current_h3lis_fs = 100;
static void double_to_string(double value, char *buffer, uint8_t precision);

static uint8_t ChannelMapping[8] = {0, 1, 2, 3, 4, 5, 6, 7};
static uint8_t User_Channel_Mapping[8] = {1, 2, 3, 4, 5, 6, 7, 8};

uint8_t CMD_sequence_index = 0;


/*************************************************
 *                Command Define                 *
 *************************************************/

static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);

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

//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);








/*************************************************
 *                 Command  Array                *
 *************************************************/


static const CliCommandBinding cliStaticBindings_internal[] = {
    { NULL, "HELP",         "format: help",                                         false, NULL, CMD_Help },
    { NULL, "RESET",        "Reset MCU: reset",                                     false, NULL, CMD_Reset },             // Giữ từ ví dụ ban đầu
    { NULL, "CLR",          "Clears the console",                                   false, NULL, CMD_ClearCLI },          // Giữ từ ví dụ ban đầu


	{ NULL,	"SET_SEQUENCE_INDEX", 			"format: SET_SEQUENCE_INDEX [N]",				true,	NULL,CMD_SET_SEQUENCE_INDEX},
	{ NULL,	"SET_SEQUENCE_DELETE", 			"format: SET_SEQUENCE_DELETE",					false,	NULL,CMD_SET_SEQUENCE_DELETE},
	{ NULL,	"SET_SEQUENCE_CONFIRM", 		"format: SET_SEQUENCE_CONFIRM",					false,	NULL,CMD_SET_SEQUENCE_CONFIRM},
	{ NULL,	"SET_SEQUENCE_DELAY", 			"format: SET_SEQUENCE_DELAY [N]",				true,	NULL,CMD_SET_SEQUENCE_DELAY},

	{ NULL,	"SET_PULSE_POLE", 				"format: SET_PULSE_POLE [N] [M]",				true,	NULL,CMD_SET_PULSE_POLE},
	{ NULL,	"SET_PULSE_COUNT", 				"format: SET_PULSE_COUNT [N] [S] [X] [Y]",		true,	NULL,CMD_SET_PULSE_COUNT},
	{ NULL,	"SET_PULSE_DELAY", 				"format: SET_PULSE_DELAY [N] [M] [X]",			true,	NULL,CMD_SET_PULSE_DELAY},
	{ NULL,	"SET_PULSE_HV_POS", 			"format: SET_PULSE_HV_POS [N] [S]",				true,	NULL,CMD_SET_PULSE_HV_POS},
	{ NULL,	"SET_PULSE_HV_NEG", 			"format: SET_PULSE_HV_NEG [N] [S]",				true,	NULL,CMD_SET_PULSE_HV_NEG},
	{ NULL,	"SET_PULSE_LV_POS", 			"format: SET_PULSE_LV_POS [N] [S]",				true,	NULL,CMD_SET_PULSE_LV_POS},
	{ NULL,	"SET_PULSE_LV_NEG", 			"format: SET_PULSE_LV_NEG [N] [S]",				true,	NULL,CMD_SET_PULSE_LV_NEG},
	{ NULL,	"SET_PULSE_CONTROL", 			"format: SET_PULSE_CONTROL [N]",				true,	NULL,CMD_SET_PULSE_CONTROL},

};

/*************************************************
 *             Command List Function             *
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

	is_h_bridge_enable = (bool*) receive_argm;
	SchedulerTaskEnable(H_BRIDGE_TASK, 1);

	return;
}


//static void	CMD_SET_CAP_VOLT_ALL(EmbeddedCli *cli, char *args, void *context){
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc < 2) {
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//		return;
//	}
//
//	else if(argc > 2){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
//		return;
//	}
//
//	int receive_argm[2];
//
//	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
//	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
//
//	if ((receive_argm[0] > 350) || (receive_argm[0] < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//	else if ((receive_argm[1] > 60) || (receive_argm[1] < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//
//	uint8_t is_return = 0;
//
//	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
//		is_return = 1;
//	}
//
//	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
//		is_return = 1;
//	}
//
//	if (is_return == 1)
//	{
//		return ;
//	}
//
//	Cap_Set_Volt_All(receive_argm[0], receive_argm[1]);
//	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
//	return;
//
//}
//static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context){
////	if (g_is_calib_running == true)
////	{
////		return ;
////	}
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc < 1) {
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//		return;
//	}
//	else if(argc > 1){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//	int receive_argm;
//	receive_argm = atoi(embeddedCliGetToken(args, 1));
//
//	if ((receive_argm > 350) || (receive_argm < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//
//	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
//		return ;
//	}
//
//	Cap_Set_Volt(CAP_PRF_HV, receive_argm);
//	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
//
//	return;
//
//
//}
//static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context){
////	if (g_is_calib_running == true)
////	{
////		return ;
////	}
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc < 1) {
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//		return;
//	}
//	else if(argc > 1){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//	int receive_argm;
//
//	receive_argm = atoi(embeddedCliGetToken(args, 1));
//
//	if ((receive_argm > 60) || (receive_argm < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//
//	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
//	{
//		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
//		return ;
//	}
//
//	Cap_Set_Volt(CAP_PRF_LV, receive_argm);
//	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
//	return;
//
//}
//static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){
//
////	if (g_is_calib_running == true)
////	{
////		return ;
////	}
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc < 2) {
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//		return;
//	}
//	else if(argc > 2){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//
//	int receive_argm[2];
//	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
//	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
//
//	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//
//	uint16_t set_volt;
//	uint16_t measure_volt;
//	uint8_t  is_return = 0;
//
//	if (receive_argm[0] == 1)
//	{
//		set_volt 	 = Cap_Get_Set_Volt(CAP_PRF_HV);
//		measure_volt = Cap_Measure_Volt(CAP_PRF_HV);
//
//		if (set_volt < measure_volt)
//		{
//			embeddedCliPrint(cli,"\n\r> CURRENT HV VOLT IS HIGHER THAN YOUR SET VOLT, PLEASE DISCHARGE BEFORE CHARGING");
//			is_return = 1;
//		}
//	}
//
//	if (receive_argm[1] == 1)
//	{
//		set_volt 	 = Cap_Get_Set_Volt(CAP_PRF_LV);
//		measure_volt = Cap_Measure_Volt(CAP_PRF_LV);
//
//		if (set_volt < measure_volt)
//		{
//			embeddedCliPrint(cli,"CURRENT LV VOLT IS HIGHER THAN YOUR SET VOLT, PLEASE DISCHARGE BEFORE CHARGING\n\r");
//			is_return = 1;
//		}
//	}
//
//	if (is_return == 1)
//	{
//		return ;
//	}
//
//	Cap_Set_Charge_All(receive_argm[0], receive_argm[1],true,true);
//	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
//
//	return ;
//}
//
//static void CMD_SET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){
//
//	int argc = embeddedCliGetTokenCount(args);
//
//	if(argc < 2) {
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//		return;
//	}
//	else if(argc > 2){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//	int receive_argm[2];
//	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
//	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
//
//	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
//		return;
//	}
//	Cap_Set_Discharge_All(receive_argm[0], receive_argm[1]);
//	embeddedCliPrint(cli,"CMDLINE_OK\n\r");
//	return ;
//
//}
//static void CMD_GET_CAP_VOLT (EmbeddedCli *cli, char *args, void *context){
//	char msg [64];
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc != 0){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
//	hv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_HV);
//	lv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_LV);
//
//	sprintf(msg, "\n\r> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV", hv_cap_set_voltage, lv_cap_set_voltage);
//	embeddedCliPrint(cli,msg);
//
//}
//static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc != 0){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT CHARGING");
//	}
//
//	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
//	{
//		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT CHARGING");
//	}
//
//}
//static void	CMD_GET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc != 0){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//	if (Cap_Get_is_Discharge(CAP_PRF_HV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS DISCHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT DISCHARGING");
//	}
//
//	if (Cap_Get_is_Discharge(CAP_PRF_LV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS DISCHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT DISCHARGING");
//	}
//
//}
//static void CMD_GET_CAP_ALL (EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	int argc = embeddedCliGetTokenCount(args);
//
//	if(argc != 0){
//		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//		return;
//	}
//
//	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
//
//	hv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_HV);
//	lv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_LV);
//
//	sprintf(msg, "\n\r> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV", hv_cap_set_voltage, lv_cap_set_voltage);
//	embeddedCliPrint(cli,msg);
//
//	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT CHARGING");
//	}
//
//	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
//	{
//		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT CHARGING");
//	}
//
//	if (Cap_Get_is_Discharge(CAP_PRF_HV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS DISCHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT DISCHARGING");
//	}
//
//	if (Cap_Get_is_Discharge(CAP_PRF_LV) == true)
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS DISCHARGING");
//	}
//	else
//	{
//		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT DISCHARGING");
//	}
//
//}

//static void CMD_GET_SENSOR_GYRO(EmbeddedCli *cli, char *args, void *context){
//
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//
//		Sensor_Read_Value(SENSOR_READ_GYRO);
//		CMD_process_state = 1;
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			return ;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return ;
//		}
//
//		sprintf(msg,"> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", Sensor_Gyro.x, Sensor_Gyro.y, Sensor_Gyro.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//		CMD_process_state = 0;
//	    return ;
//	}
//	default:
//		break;
//	} //switch
//
//	return;
//}
//static void CMD_GET_SENSOR_ACCEL(EmbeddedCli *cli, char *args, void *context){
//
//	char msg[100];
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_ACCEL);
//		CMD_process_state = 1;
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		sprintf(msg, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", Sensor_Accel.x, Sensor_Accel.y, Sensor_Accel.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//
//		CMD_process_state = 0;
//	    return;
//	}
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_LSM6DSOX(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_LSM6DSOX);
//		CMD_process_state = 1;
//		return;
//	}
//
//
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		sprintf(msg, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", Sensor_Gyro.x, Sensor_Gyro.y, Sensor_Gyro.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//		sprintf(msg, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", Sensor_Accel.x, Sensor_Accel.y, Sensor_Accel.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//		CMD_process_state = 0;
//	    return;
//	}
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_TEMP(EmbeddedCli *cli, char *args, void *context){
//
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_TEMP);
//		CMD_process_state = 1;
//		embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_BMP390_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		char fractional_string[16] = {0};
//		double_to_string(Sensor_Temp, fractional_string, 3);
//
//		sprintf(msg, "> TEMPERATURE: %s C\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		CMD_process_state = 0;
//	    return;
//	}
//
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_PRESSURE(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_PRESSURE);
//		CMD_process_state = 1;
//		embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_BMP390_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		char fractional_string[16] = {0};
//		double_to_string(Sensor_Pressure, fractional_string, 3);
//
//		sprintf(msg, "\r\n> PRESSURE: %s Pa\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		CMD_process_state = 0;
//	    return;
//	}
//
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_ALTITUDE(EmbeddedCli *cli, char *args, void *context){
//	embeddedCliPrint(cli, "\r\n> Tobe continued");
//}
//static void CMD_GET_SENSOR_BMP390(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_BMP390);
//		CMD_process_state = 1;
//		embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_BMP390_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		char fractional_string[16] = {0};
//
//		double_to_string(Sensor_Temp, fractional_string, 3);
//		sprintf(msg, "\r\n> TEMPERATURE: %s C\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		double_to_string(Sensor_Pressure, fractional_string, 3);
//		sprintf(msg, "\r\n> PRESSURE: %s Pa\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		// double_to_string(Sensor_Altitude, fractional_string, 3);
//		// UART_Printf(CMD_line_handle, "> ALTITUDE: %s m\n", fractional_string);
//
//		CMD_process_state = 0;
//	    return;
//	}
//
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_H3LIS(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	switch (CMD_process_state)
//		{
//		case 0:
//		{
//			int argc = embeddedCliGetTokenCount(args);
//			if(argc != 0){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//				return;
//			}
//
//			Sensor_Read_Value(ONBOARD_SENSOR_READ_H3LIS331DL);
//			CMD_process_state = 1;
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		case 1:
//		{
//			i2c_result_t return_value = Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb);
//
//			if (return_value == I2C_IS_RUNNING)
//			{
//				embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//				return;
//			}
//			else if (return_value != I2C_OK)
//			{
//				CMD_process_state = 0;
//				return;
//			}
//
//			sprintf(msg, "\r\n> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", H3LIS_Accel.x, H3LIS_Accel.y, H3LIS_Accel.z);
//			embeddedCliPrint(cli,msg);
//			CMD_process_state = 0;
//			return CMDLINE_OK;
//		}
//		default:
//			break;
//		}
//
//		return;
//}
//static void CMD_SET_SENSOR_H3LIS_FS(EmbeddedCli *cli, char *args, void *context){
//	char ms[100];
//	switch (CMD_process_state)
//		{
//		case 0:
//		{
//			int argc = embeddedCliGetTokenCount(args);
//			if(argc > 1){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//				return;
//			}
//			else if(argc <1){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//				return;
//			}
//
//			int receive_argm;
//			receive_argm  = atoi(embeddedCliGetToken(args, 1));
//
//			if ((receive_argm != 100) && (receive_argm != 200) && (receive_argm != 400))
//			{
//				embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARGS");
//				return CMDLINE_INVALID_ARG;
//			}
//
//			switch (receive_argm)
//			{
//			case 100:
//				Sensor_Read_Value(ONBOARD_SENSOR_SET_FS_100G);
//				break;
//			case 200:
//				Sensor_Read_Value(ONBOARD_SENSOR_SET_FS_200G);
//				break;
//			case 400:
//				Sensor_Read_Value(ONBOARD_SENSOR_SET_FS_400G);
//				break;
//
//			default:
//				break;
//			}
//
//			current_h3lis_fs = receive_argm;
//			CMD_process_state = 1;
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//
//		case 1:
//		{
//			i2c_result_t return_value = Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb);
//
//			if (return_value == I2C_IS_RUNNING)
//			{
//				embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//				return;
//			}
//			else if (return_value != I2C_OK)
//			{
//				CMD_process_state = 0;
//				return;
//			}
//
//			embeddedCliPrint(cli, "\n\r> SET FULL SCALE FOR H3LIS331DL SUCCESSFULLY\n");
//			CMD_process_state = 0;
//			return;
//		}
//
//		default:
//			break;
//		}
//
//		return;
//}
//static void CMD_GET_SENSOR_H3LIS_FS(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	switch (CMD_process_state)
//		{
//		case 0:
//		{
//			int argc = embeddedCliGetTokenCount(args);
//			if(argc != 0){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//				return;
//			}
//
//			Sensor_Read_Value(ONBOARD_SENSOR_GET_FS);
//			CMD_process_state = 1;
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//
//		case 1:
//		{
//			i2c_result_t return_value = Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb);
//
//			if (return_value == I2C_IS_RUNNING)
//			{
//				embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//				return;
//			}
//			else if (return_value != I2C_OK)
//			{
//				CMD_process_state = 0;
//				return;
//			}
//
//			uint16_t fs = 0;
//
//			switch (H3LIS_Accel.full_scale)
//			{
//			case 0:
//				fs = 100;
//				break;
//			case 1:
//				fs = 200;
//				break;
//			case 3:
//				fs = 400;
//
//			default:
//				break;
//			}
//
//			sprintf(msg,"> H3LIS331DL FULL SCALE: %dG\n", fs);
//			embeddedCliPrint(cli,msg);
//			current_h3lis_fs = fs;
//			CMD_process_state = 0;
//			return;
//		}
//
//		default:
//			break;
//		}
//
//		return;
//}
//
//static void CMD_GET_SENSOR_ADS1115_BAT(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc != 0){
//		embeddedCliPrint(cli, "\r\n> CMD_TOO_MANY_ARG");
//		rerturn;
//	}
//
//
//}
//static void CMD_GET_SENSOR_ADS1115_AC(EmbeddedCli *cli, char *args, void *context){
//
//}
//
//static void CMD_GET_SENSOR_ADS1115_CHAR(EmbeddedCli *cli, char *args, void *context){
//
//}



/*************************************************
 *                  End CMD List                 *
 *************************************************/
static void double_to_string(double value, char *buffer, uint8_t precision)
{
    // Handle negative numbers
    if (value < 0)
	{
        *buffer++ = '-';
        value = -value;
    }

    // Extract the integer part
    uint32_t integer_part  = (uint32_t)value;
    double fractional_part = value - integer_part;

    // Convert integer part to string
    sprintf(buffer, "%ld", integer_part);
    while (*buffer) buffer++; // Move pointer to the end of the integer part

    // Add decimal point
    if (precision > 0)
	{
        *buffer++ = '.';

        // Extract and convert the fractional part
        for (uint8_t i = 0; i < precision; i++)
		{
            fractional_part *= 10;
            uint8_t digit = (uint8_t)fractional_part;
            *buffer++ = '0' + digit;
            fractional_part -= digit;
        }
    }

    // Null-terminate the string
    *buffer = '\0';
}

/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
