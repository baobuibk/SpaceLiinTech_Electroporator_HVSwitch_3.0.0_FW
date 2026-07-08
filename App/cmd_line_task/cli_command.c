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
#include "crc.h"

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
uint8_t is_measure_volt_notify_enable = 0;


/*************************************************
 *                Command Define                 *
 *************************************************/
static void fsp_print(uint8_t packet_length);

static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_SYSTEM_RESET_GP_SW(EmbeddedCli *cli, char *args, void *context);
static void CMD_SYSTEM_RESET_GP_CTRL(EmbeddedCli *cli, char *args, void *context);
static void CMD_SYSTEM_RESET_GP_ALL(EmbeddedCli *cli, char *args, void *context);

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

/*----------------------CMD FOR MEASURE--------------------------------*/
static void CMD_MEASURE_VOLT (EmbeddedCli *cli, char *args, void *context);
static void CMD_MEASURE_IMPEDANCE (EmbeddedCli *cli, char *args, void *context);

static void CMD_SET_CURRENT_LIMIT (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_CURRENT_LIMIT (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_OVV_FLAG (EmbeddedCli *cli, char *args, void *context);
static void CMD_RESET_OVV_FLAG_HV (EmbeddedCli *cli, char *args, void *context);
static void CMD_RESET_OVV_FLAG_LV (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_OVC_FLAG (EmbeddedCli *cli, char *args, void *context);
static void CMD_RESET_OVC_FLAG (EmbeddedCli *cli, char *args, void *context);

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


static void	CMD_GET_SEQUENCE_INDEX(EmbeddedCli *cli, char *args, void *context);
static void	CMD_GET_SEQUENCE_DELETE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SEQUENCE_CONFIRM (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SEQUENCE_DELAY (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SEQUENCE_ALL (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_COUNT (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_DELAY (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_HV (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_HV_POS (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_HV_NEG (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_LV (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_LV_POS (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_LV_NEG (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_CONTROL (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_PULSE_ALL (EmbeddedCli *cli, char *args, void *context);

/*----------------------CMD FOR ACCEL PULSING -----------------------------*/


/*----------------------CMD FOR READ SENSOR-----------------------------*/
static void CMD_GET_SENSOR_GYRO (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_ACCEL (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_LSM6DSOX (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_SENSOR_TEMP (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_PRESSURE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_ALTITUDE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_BMP390 (EmbeddedCli *cli, char *args, void *context);

static void CMD_SET_SENSOR_H3LIS_FS (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_H3LIS_VALUE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_H3LIS_FS (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_SENSOR_HV_TEMP (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_SENSOR_LV_TEMP (EmbeddedCli *cli, char *args, void *context);


/*----------------------CMD FOR DEBUG -----------------------------*/



/*************************************************
 *                 Command  Array                *
 *************************************************/


static const CliCommandBinding cliStaticBindings_internal[] = {
	/*------------------------------------------ SYSTEM CMD --------------------------------------------------------------------- */
    { NULL, "HELP",         			"format: help",                                     false, NULL, CMD_Help },
    { NULL, "SYSTEM_RESET_GP_ALL",      "Reset all system: reset",                          false, NULL, CMD_SYSTEM_RESET_GP_ALL },
	{ NULL, "SYSTEM_RESET_GP_SW",      	"Reset SW system: reset",                          	false, NULL, CMD_SYSTEM_RESET_GP_SW },
	{ NULL, "SYSTEM_RESET_GP_CTRL",     "Reset CTRL system: reset",                         false, NULL, CMD_SYSTEM_RESET_GP_CTRL },
    { NULL, "CLR",         				"Clears the console",                               false, NULL, CMD_ClearCLI },

	/*------------------------------------------ CAP CONTROL CMD ----------------------------------------------------------------- */
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

	/*------------------------------------------ MEASURE CMD --------------------------------------------------------------------- */
	{ NULL,	"MEASURE_VOLT", 			"format: MEASURE_VOLT",								false,NULL,CMD_MEASURE_VOLT},
	{ NULL,	"MEASURE_IMPEDANCE", 		"format: MEASURE_IMPEDANCE [N] [M]",				true,NULL,CMD_MEASURE_IMPEDANCE},

	/*------------------------------------------ CURRENT CONTROL CMD -------------------------------------------------------------- */
	{ NULL,	"SET_CURRENT_LIMIT", 		"format: SET_CURRENT_LIMIT [N] [S]",				true,NULL,CMD_SET_CURRENT_LIMIT},
	{ NULL,	"GET_CURRENT_LIMIT", 		"format: GET_CURRENT_LIMIT",						false,NULL,CMD_GET_CURRENT_LIMIT},

	/*------------------------------------------ ERROR HANDLE CMD ----------------------------------------------------------------- */
	{ NULL,	"GET_OVV_FLAG", 			"format: GET_OVV_FLAG",								false,NULL,CMD_GET_OVV_FLAG},
	{ NULL,	"RESET_OVV_FLAG_HV", 		"format: RESET_OVV_FLAG_HV",						false,NULL,CMD_RESET_OVV_FLAG_HV},
	{ NULL,	"RESET_OVV_FLAG_LV", 		"format: RESET_OVV_FLAG_LV",						false,NULL,CMD_RESET_OVV_FLAG_LV},
	{ NULL,	"GET_OVC_FLAG", 			"format: GET_OVC_FLAG",								false,NULL,CMD_GET_OVC_FLAG},
	{ NULL,	"RESET_OVC_FLAG", 			"format: RESET_OVC_FLAG",							false,NULL,CMD_RESET_OVC_FLAG},

	/*------------------------------------------ H BRIDGE PULSE CMD --------------------------------------------------------------- */
	{ NULL,	"SET_SEQUENCE_INDEX", 		"format: SET_SEQUENCE_INDEX [N]",					true,	NULL,CMD_SET_SEQUENCE_INDEX},
	{ NULL,	"SET_SEQUENCE_DELETE", 		"format: SET_SEQUENCE_DELETE",						false,	NULL,CMD_SET_SEQUENCE_DELETE},
	{ NULL,	"SET_SEQUENCE_CONFIRM", 	"format: SET_SEQUENCE_CONFIRM",						false,	NULL,CMD_SET_SEQUENCE_CONFIRM},
	{ NULL,	"SET_SEQUENCE_DELAY", 		"format: SET_SEQUENCE_DELAY [N]",					true,	NULL,CMD_SET_SEQUENCE_DELAY},

	{ NULL,	"SET_PULSE_POLE", 			"format: SET_PULSE_POLE [N] [M]",					true,	NULL,CMD_SET_PULSE_POLE},
	{ NULL,	"SET_PULSE_COUNT", 			"format: SET_PULSE_COUNT [N] [S] [X] [Y]",			true,	NULL,CMD_SET_PULSE_COUNT},
	{ NULL,	"SET_PULSE_DELAY", 			"format: SET_PULSE_DELAY [N] [M] [X]",				true,	NULL,CMD_SET_PULSE_DELAY},
	{ NULL,	"SET_PULSE_HV_POS", 		"format: SET_PULSE_HV_POS [N] [S]",					true,	NULL,CMD_SET_PULSE_HV_POS},
	{ NULL,	"SET_PULSE_HV_NEG", 		"format: SET_PULSE_HV_NEG [N] [S]",					true,	NULL,CMD_SET_PULSE_HV_NEG},
	{ NULL,	"SET_PULSE_LV_POS", 		"format: SET_PULSE_LV_POS [N] [S]",					true,	NULL,CMD_SET_PULSE_LV_POS},
	{ NULL,	"SET_PULSE_LV_NEG", 		"format: SET_PULSE_LV_NEG [N] [S]",					true,	NULL,CMD_SET_PULSE_LV_NEG},
	{ NULL,	"SET_PULSE_CONTROL", 		"format: SET_PULSE_CONTROL [N]",					true,	NULL,CMD_SET_PULSE_CONTROL},

	{ NULL,	"GET_SEQUENCE_INDEX", 		"format: GET_SEQUENCE_INDEX ",						false,	NULL,CMD_GET_SEQUENCE_INDEX},
	{ NULL,	"GET_SEQUENCE_DELETE", 		"format: GET_SEQUENCE_DELETE",						false,	NULL,CMD_GET_SEQUENCE_DELETE},
	{ NULL,	"GET_SEQUENCE_CONFIRM", 	"format: GET_SEQUENCE_CONFIRM",						false,	NULL,CMD_GET_SEQUENCE_CONFIRM},
	{ NULL,	"GET_SEQUENCE_DELAY", 		"format: GET_SEQUENCE_DELAY",						false,	NULL,CMD_GET_SEQUENCE_DELAY},
	{ NULL,	"GET_SEQUENCE_ALL", 		"format: GET_SEQUENCE_ALL",							false,	NULL,CMD_GET_SEQUENCE_ALL},

	{ NULL,	"GET_PULSE_POLE", 			"format: GET_PULSE_POLE",							false,	NULL,CMD_GET_PULSE_POLE},
	{ NULL,	"GET_PULSE_COUNT", 			"format: GET_PULSE_COUNT",							false,	NULL,CMD_GET_PULSE_COUNT},
	{ NULL,	"GET_PULSE_DELAY", 			"format: GET_PULSE_DELAY",							false,	NULL,CMD_GET_PULSE_DELAY},
	{ NULL,	"GET_PULSE_HV", 			"format: GET_PULSE_HV",								false,	NULL,CMD_GET_PULSE_HV},
	{ NULL,	"GET_PULSE_HV_POS", 		"format: GET_PULSE_HV_POS",							false,	NULL,CMD_GET_PULSE_HV_POS},
	{ NULL,	"GET_PULSE_HV_NEG", 		"format: GET_PULSE_HV_NEG",							false,	NULL,CMD_GET_PULSE_HV_NEG},
	{ NULL,	"GET_PULSE_LV", 			"format: GET_PULSE_LV",								false,	NULL,CMD_GET_PULSE_LV},
	{ NULL,	"GET_PULSE_LV_POS", 		"format: GET_PULSE_LV_POS",							false,	NULL,CMD_GET_PULSE_LV_POS},
	{ NULL,	"GET_PULSE_LV_NEG", 		"format: GET_PULSE_LV_NEG",							false,	NULL,CMD_GET_PULSE_LV_NEG},
	{ NULL,	"GET_PULSE_CONTROL", 		"format: GET_PULSE_CONTROL",						false,	NULL,CMD_GET_PULSE_CONTROL},
	{ NULL,	"GET_PULSE_ALL", 			"format: GET_PULSE_ALL",							false,	NULL,CMD_GET_PULSE_ALL},

	/*------------------------------------------ SENSOR CMD --------------------------------------------------------------------- */
	{ NULL,	"GET_SENSOR_GYRO", 			"format: GET_SENSOR_GYRO",							false,	NULL,CMD_GET_SENSOR_GYRO},
	{ NULL,	"GET_SENSOR_ACCEL", 		"format: GET_SENSOR_ACCEL",							false,	NULL,CMD_GET_SENSOR_ACCEL},
	{ NULL,	"GET_SENSOR_LSM6DSOX_DATA",	"format: GET_SENSOR_LSM6DSOX_DATA",					false,	NULL,CMD_GET_SENSOR_LSM6DSOX},

	{ NULL,	"GET_SENSOR_TEMP", 			"format: GET_SENSOR_TEMP",							false,	NULL,CMD_GET_SENSOR_TEMP},
	{ NULL,	"GET_SENSOR_PRESSURE", 		"format: GET_SENSOR_PRESSURE",						false,	NULL,CMD_GET_SENSOR_PRESSURE},
	{ NULL,	"GET_SENSOR_ALTITUDE", 		"format: GET_SENSOR_ALTITUDE",						false,	NULL,CMD_GET_SENSOR_ALTITUDE},
	{ NULL,	"GET_SENSOR_BMP390_DATA", 	"format: GET_SENSOR_BMP390_DATA",					false,	NULL,CMD_GET_SENSOR_BMP390},

	{ NULL,	"GET_SENSOR_H3LIS_DATA", 	"format: GET_SENSOR_H3LIS_VALUE",					false,	NULL,CMD_GET_SENSOR_H3LIS_VALUE},
	{ NULL,	"SET_SENSOR_H3LIS_FS", 		"format: SET_SENSOR_H3LIS_FS [N]",					true,	NULL,CMD_SET_SENSOR_H3LIS_FS},
	{ NULL,	"GET_SENSOR_H3LIS_FS", 		"format: GET_SENSOR_H3LIS_FS",						false,	NULL,CMD_GET_SENSOR_H3LIS_FS},

	{ NULL,	"GET_SENSOR_HV_TEMP", 		"format: GET_SENSOR_HV_TEMP",						false,	NULL,CMD_GET_SENSOR_HV_TEMP},
	{ NULL,	"GET_SENSOR_LV_TEMP", 		"format: GET_SENSOR_LV_TEMP",						false,	NULL,CMD_GET_SENSOR_LV_TEMP},
};

/*************************************************
 *             Command List Function             *
 *************************************************/

/*----------------------CMD FOR CAP CONTROL--------------------------------*/

static void	CMD_SET_CAP_VOLT_ALL(EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);

	if(argc < 2) {
		embeddedCliPrint(cli,"> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 300) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 50) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_ALL;
	ps_FSP_TX -> Payload.set_volt_all.HV_High = (uint8_t) ((receive_argm[0]>>8) & 0xFF);
	ps_FSP_TX -> Payload.set_volt_all.HV_Low = (uint8_t) (receive_argm[0] & 0xFF);
	ps_FSP_TX -> Payload.set_volt_all.LV_High = (uint8_t) ((receive_argm[1]>>8) & 0xFF);;
	ps_FSP_TX -> Payload.set_volt_all.LV_Low = (uint8_t) (receive_argm[1] & 0xFF);

	fsp_print(5);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc < 1) {
		embeddedCliPrint(cli,"> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 1){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t receive_argm = atoi(embeddedCliGetToken(args, 1));

	if ((receive_argm > 300) || (receive_argm < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_HV;
	ps_FSP_TX -> Payload.set_volt_hv.HV_High = (uint8_t) ((receive_argm>>8) & 0xFF);
	ps_FSP_TX -> Payload.set_volt_hv.HV_Low = (uint8_t) (receive_argm & 0xFF);

	fsp_print(3);

	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc < 1) {
		embeddedCliPrint(cli,"> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 1){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t receive_argm = atoi(embeddedCliGetToken(args, 1));

	if ((receive_argm > 50) || (receive_argm < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_LV;
	ps_FSP_TX -> Payload.set_volt_lv.LV_High = (uint8_t) ((receive_argm>>8) & 0xFF);
	ps_FSP_TX -> Payload.set_volt_lv.LV_Low = (uint8_t) (receive_argm & 0xFF);

	fsp_print(3);

	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc < 2) {
		embeddedCliPrint(cli,"> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_CONTROL;
	ps_FSP_TX -> Payload.set_charge.HV_cmd_charge = receive_argm[0];
	ps_FSP_TX -> Payload.set_charge.LV_cmd_charge = receive_argm[1];

	fsp_print(3);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return ;
}

static void CMD_SET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);

	if(argc < 2) {
		embeddedCliPrint(cli,"> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_RELEASE;
	ps_FSP_TX -> Payload.set_discharge.HV_cmd_discharge = receive_argm[0];
	ps_FSP_TX -> Payload.set_discharge.LV_cmd_discharge = receive_argm[1];

	fsp_print(3);
	embeddedCliPrint(cli,"> CMDLINE_OK");
	return ;
}

static void CMD_RESET_OVV_FLAG_HV (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_RESET_CAP_OVV;
	ps_FSP_TX -> Payload.reset_ovv_flag.HV_OVV_flag = 0;
	ps_FSP_TX -> Payload.reset_ovv_flag.LV_OVV_flag = 1;
	fsp_print(3);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return ;
}

static void CMD_RESET_OVV_FLAG_LV (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_RESET_CAP_OVV;
	ps_FSP_TX -> Payload.reset_ovv_flag.HV_OVV_flag = 1;
	ps_FSP_TX -> Payload.reset_ovv_flag.LV_OVV_flag = 0;
	fsp_print(3);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return ;
}

static void CMD_GET_CAP_VOLT (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_VOLT;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;         
}

static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_CONTROL;
	fsp_print(1);
	
	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void	CMD_GET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_RELEASE;
	fsp_print(1);
	
	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void CMD_GET_CAP_ALL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_ALL;
	fsp_print(1);
	
	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void CMD_GET_CAP_STATE (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_STATE;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void CMD_GET_OVV_FLAG (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_OVV_FLAG;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

/*----------------------CMD FOR MEASURE -----------------------------*/

static void CMD_MEASURE_VOLT (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_MEASURE_VOLT;
	fsp_print(1);
	
	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void CMD_MEASURE_IMPEDANCE (EmbeddedCli *cli, char *args, void *context){

	if(H_Bridge_State != HB_TASK_IDLE){
		embeddedCliPrint(cli, "> H BRIDGE PULSING IS IN PROGRESS");
		return;
	}

	uint8_t argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	uint8_t pos_pole = ChannelMapping[receive_argm[0] - 1];
	uint8_t neg_pole = ChannelMapping[receive_argm[1] - 1];

	uint16_t impedance_val =  impedance_measure_manual(pos_pole, neg_pole);

	char msg[64];
	sprintf(msg, "> IMPEDANCE IS %d Ohm", impedance_val);
	embeddedCliPrint(cli,msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_SET_CURRENT_LIMIT (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 11) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	if ((receive_argm[1] > 9) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	if ((receive_argm[0] == 11) && (receive_argm[1] > 7)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	if ((receive_argm[0] == 0) && (receive_argm[1] == 0)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	float current_limit = (float)receive_argm[0] + ((float)receive_argm[1] * 0.1f);
	VOM_Set_Current_Limit(current_limit);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_CURRENT_LIMIT (EmbeddedCli *cli, char *args, void *context){

	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	float current_limit = VOM_Get_Current_Limit() * 1000.0f; // Convert to mA

	char msg[64];
	sprintf(msg, "> OUTPUT CURRENT LIMIT SET AT: %d mA", (int)current_limit);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_OVC_FLAG (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	bool ovc_flag = VOM_Get_OVC_Flag();

	if(ovc_flag){
		embeddedCliPrint(cli, "> OVER CURRENT FLAG STATUS IS TRUE");
	} else {
		embeddedCliPrint(cli, "> OVER CURRENT FLAG STATUS IS FALSE");
	}

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_RESET_OVC_FLAG (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_TOO_MANY_ARGS");
		return;
	}
	VOM_Reset_OVC_Flag();
	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}


/*----------------------CMD FOR H BRIDGE CONTROL-----------------------------*/
static void	CMD_SET_SEQUENCE_INDEX(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if (argc > 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t user_target_seq_idx;
	user_target_seq_idx = atoi(embeddedCliGetToken(args, 1));

	if (user_target_seq_idx < 1 || user_target_seq_idx > MAX_SEQUENCES){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	if(user_target_seq_idx > (CMD_sequence_index + 2)){
		char msg[128];
		sprintf(msg,"> ERROR YOUR NEXT SEQUENCE INDEX IS: %d, NOT %d\n", total_active_sequences + 1, user_target_seq_idx);
		UART_Driver_SendString(&XBEE_UART, msg);
		
		sprintf(msg,"> > CURRENT SEQUENCE INDEX: %d\n", total_active_sequences);
		UART_Driver_SendString(&XBEE_UART, msg);
		return;
	}

	CMD_sequence_index = user_target_seq_idx - 1;

	Sequence_List[CMD_sequence_index].is_edit_enable = true;

	total_active_sequences = CMD_sequence_index + 1;

	embeddedCliPrint(cli,"> CMDLINE_OK");

	return;
}

static void	CMD_SET_SEQUENCE_DELETE(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
	}

	Sequence_List[CMD_sequence_index].is_edit_enable = false;
	total_active_sequences = CMD_sequence_index--;

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void	CMD_SET_SEQUENCE_CONFIRM(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	Sequence_List[CMD_sequence_index].is_edit_enable = false;
	Sequence_List[CMD_sequence_index].is_confirm	 = true;

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void	CMD_SET_SEQUENCE_DELAY(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	uint8_t receive_argm = atoi(embeddedCliGetToken(args, 1));
	Sequence_List[CMD_sequence_index].sequence_delay_ms = receive_argm;

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}


static void	CMD_SET_PULSE_POLE(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint8_t receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].pos_pole_index =  ChannelMapping[receive_argm[0] - 1];
	Sequence_List[CMD_sequence_index].neg_pole_index =  ChannelMapping[receive_argm[1] - 1];

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_COUNT(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 4) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 4) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	int receive_argm[4];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
	receive_argm[2] = atoi(embeddedCliGetToken(args, 3));
	receive_argm[3] = atoi(embeddedCliGetToken(args, 4));

	if ((receive_argm[0] > 20) || (receive_argm[1] > 20) || (receive_argm[2] > 20) || (receive_argm[3] > 20)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	Sequence_List[CMD_sequence_index].hv_pos_count 	= receive_argm[0];
	Sequence_List[CMD_sequence_index].hv_neg_count 	= receive_argm[1];

    Sequence_List[CMD_sequence_index].lv_pos_count 	= receive_argm[2];
    Sequence_List[CMD_sequence_index].lv_neg_count 	= receive_argm[3];

	embeddedCliPrint(cli,"> CMDLINE_OK");
    return;
}

static void	CMD_SET_PULSE_DELAY(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 3) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 3) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	if (Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	int receive_argm[3];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));
	receive_argm[2] = atoi(embeddedCliGetToken(args, 3));

	Sequence_List[CMD_sequence_index].hv_delay_ms = receive_argm[0];
	Sequence_List[CMD_sequence_index].lv_delay_ms = receive_argm[1];
	Sequence_List[CMD_sequence_index].pulse_delay_ms = receive_argm[2];

	embeddedCliPrint(cli, "> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_HV_POS(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	Sequence_List[CMD_sequence_index].hv_pos_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].hv_pos_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "> CMDLINE_OK");

	return;
}

static void	CMD_SET_PULSE_HV_NEG(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].hv_neg_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].hv_neg_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_LV_POS(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].lv_pos_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].lv_pos_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_LV_NEG(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if (argc < 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 2) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_edit_enable == false) return;

	Sequence_List[CMD_sequence_index].lv_neg_on_ms   = receive_argm[0];
	Sequence_List[CMD_sequence_index].lv_neg_off_ms  = receive_argm[1];

	embeddedCliPrint(cli, "> CMDLINE_OK");
	return;
}

static void	CMD_SET_PULSE_CONTROL(EmbeddedCli *cli, char *args, void *context){

	if(VOM_Get_OVC_Flag() == true){
		embeddedCliPrint(cli, "> OVC FLAG STATUS IS TRUE, PLEASE RESET OVC FLAG FIRST");
		return;
	}

	if(is_impedance_task_enable == true){
		embeddedCliPrint(cli, "> IMPEDANCE MEASURE TASK IS IN PROGRESS");
		return;
	}

	uint8_t argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	if(Sequence_List[CMD_sequence_index].is_confirm == false) {
		embeddedCliPrint(cli, "> PLEASE COMFIRM CURRENT SEQUENCE");
		return;
	}

	uint8_t receive_argm = atoi(embeddedCliGetToken(args, 1));

	if (receive_argm != 1)
		return;

	H_Bridge_State = HB_TASK_INIT_STATE;
	SchedulerTaskEnable(H_BRIDGE_TASK, 1);

	return;
}

static void CMD_GET_SEQUENCE_ALL (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	uint8_t seq_idx = 0;
	char msg[128];

	for(seq_idx = 0; seq_idx <= CMD_sequence_index; seq_idx++){
		embeddedCliPrint(cli,"-------------------------------------------------------------");
		sprintf(msg,"> CURRENT SEQUENCE INDEX: %d", seq_idx+1);
		embeddedCliPrint(cli, msg);

		sprintf(msg,"> PULSE POS POLE: %d; PULSE NEG POLE: %d", Sequence_List[seq_idx].pos_pole_index + 1, Sequence_List[seq_idx].neg_pole_index + 1);
		embeddedCliPrint(cli, msg);

		sprintf(msg,"> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d", Sequence_List[seq_idx].hv_pos_count, Sequence_List[seq_idx].hv_neg_count);
		embeddedCliPrint(cli, msg);

		sprintf(msg,"> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d", Sequence_List[seq_idx].lv_pos_count, Sequence_List[seq_idx].lv_pos_count);
		embeddedCliPrint(cli, msg);

		sprintf(msg,"> DELAY BETWEEN HV POS AND NEG PULSE: %dms", Sequence_List[seq_idx].hv_delay_ms);
		embeddedCliPrint(cli, msg);

		sprintf(msg,"> DELAY BETWEEN LV POS AND NEG PULSE: %dms", Sequence_List[seq_idx].lv_delay_ms);
		embeddedCliPrint(cli, msg);

		sprintf(msg,"> DELAY BETWEEN HV PULSE AND LV PULSE: %dms", Sequence_List[seq_idx].pulse_delay_ms);
		embeddedCliPrint(cli, msg);

		sprintf(msg, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms",Sequence_List[seq_idx].hv_pos_on_ms, Sequence_List[seq_idx].hv_pos_off_ms);
		embeddedCliPrint(cli, msg);

		sprintf(msg, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms",Sequence_List[seq_idx].hv_neg_on_ms, Sequence_List[seq_idx].hv_neg_off_ms);
		embeddedCliPrint(cli, msg);

		sprintf(msg, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms",Sequence_List[seq_idx].lv_pos_on_ms, Sequence_List[seq_idx].lv_pos_off_ms);
		embeddedCliPrint(cli, msg);

		sprintf(msg, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms",Sequence_List[seq_idx].lv_neg_on_ms, Sequence_List[seq_idx].lv_neg_off_ms);
		embeddedCliPrint(cli, msg);
	}
}

static void	CMD_GET_SEQUENCE_INDEX(EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg,"> CURRENT SEQUENCE INDEX: %d", CMD_sequence_index + 1);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void	CMD_GET_SEQUENCE_DELETE (EmbeddedCli *cli, char *args, void *context){
	return;
}

static void CMD_GET_SEQUENCE_CONFIRM (EmbeddedCli *cli, char *args, void *context){
	return;
}

static void CMD_GET_SEQUENCE_DELAY (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg,"> CURRENT SEQUENCE DELAY: %d", Sequence_List[CMD_sequence_index].sequence_delay_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}


static void CMD_GET_PULSE_POLE (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg,"> PULSE POS POLE: %d; PULSE NEG POLE: %d", Sequence_List[CMD_sequence_index].pos_pole_index + 1, Sequence_List[CMD_sequence_index].neg_pole_index + 1);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_PULSE_COUNT (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg,"> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d", Sequence_List[CMD_sequence_index].hv_pos_count, Sequence_List[CMD_sequence_index].hv_neg_count);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d", Sequence_List[CMD_sequence_index].lv_pos_count, Sequence_List[CMD_sequence_index].lv_pos_count);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_PULSE_DELAY (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg,"> DELAY BETWEEN HV POS AND NEG PULSE: %dms", Sequence_List[CMD_sequence_index].hv_delay_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> DELAY BETWEEN LV POS AND NEG PULSE: %dms", Sequence_List[CMD_sequence_index].lv_delay_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> DELAY BETWEEN HV PULSE AND LV PULSE: %dms", Sequence_List[CMD_sequence_index].pulse_delay_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_PULSE_HV (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	char msg[64];
	sprintf(msg, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms",Sequence_List[CMD_sequence_index].hv_pos_on_ms, Sequence_List[CMD_sequence_index].hv_pos_off_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms",Sequence_List[CMD_sequence_index].hv_neg_on_ms, Sequence_List[CMD_sequence_index].hv_neg_off_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_PULSE_HV_POS (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms",Sequence_List[CMD_sequence_index].hv_pos_on_ms, Sequence_List[CMD_sequence_index].hv_pos_off_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_PULSE_HV_NEG (EmbeddedCli *cli, char *args, void *context){
		int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	char msg[64];
	sprintf(msg, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms",Sequence_List[CMD_sequence_index].hv_neg_on_ms, Sequence_List[CMD_sequence_index].hv_neg_off_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_PULSE_LV (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	char msg[64];
	sprintf(msg, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms",Sequence_List[CMD_sequence_index].lv_pos_on_ms, Sequence_List[CMD_sequence_index].lv_pos_off_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms",Sequence_List[CMD_sequence_index].lv_neg_on_ms, Sequence_List[CMD_sequence_index].lv_neg_off_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_PULSE_LV_POS (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	char msg[64];
	sprintf(msg, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms",Sequence_List[CMD_sequence_index].lv_pos_on_ms, Sequence_List[CMD_sequence_index].lv_pos_off_ms);
	UART_Driver_SendString(&XBEE_UART, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_PULSE_LV_NEG (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	char msg[64];
	sprintf(msg, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms",Sequence_List[CMD_sequence_index].lv_neg_on_ms, Sequence_List[CMD_sequence_index].lv_neg_off_ms);
	embeddedCliPrint(cli, msg);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_PULSE_CONTROL (EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	if(H_Bridge_State != HB_TASK_IDLE){
		embeddedCliPrint(cli,"> H BRIDGE IS PULSING");
	}
	else if(H_Bridge_State == HB_TASK_IDLE){
		embeddedCliPrint(cli,"> H BRIDGE IS NOT PULSING");
	}
	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_PULSE_ALL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}
	char msg[128];

	embeddedCliPrint(cli,"-------------------------------------------------------------");
	sprintf(msg,"> CURRENT SEQUENCE INDEX: %d", CMD_sequence_index + 1);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> PULSE POS POLE: %d; PULSE NEG POLE: %d", Sequence_List[CMD_sequence_index].pos_pole_index + 1, Sequence_List[CMD_sequence_index].neg_pole_index + 1);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d", Sequence_List[CMD_sequence_index].hv_pos_count, Sequence_List[CMD_sequence_index].hv_neg_count);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d", Sequence_List[CMD_sequence_index].lv_pos_count, Sequence_List[CMD_sequence_index].lv_pos_count);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> DELAY BETWEEN HV POS AND NEG PULSE: %dms", Sequence_List[CMD_sequence_index].hv_delay_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> DELAY BETWEEN LV POS AND NEG PULSE: %dms", Sequence_List[CMD_sequence_index].lv_delay_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg,"> DELAY BETWEEN HV PULSE AND LV PULSE: %dms", Sequence_List[CMD_sequence_index].pulse_delay_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg, "> HV PULSE POS ON TIME: %dms; HV PULSE POS OFF TIME: %dms",Sequence_List[CMD_sequence_index].hv_pos_on_ms, Sequence_List[CMD_sequence_index].hv_pos_off_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg, "> HV PULSE NEG ON TIME: %dms; HV PULSE NEG OFF TIME: %dms",Sequence_List[CMD_sequence_index].hv_neg_on_ms, Sequence_List[CMD_sequence_index].hv_neg_off_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg, "> LV PULSE POS ON TIME: %dms; LV PULSE POS OFF TIME: %dms",Sequence_List[CMD_sequence_index].lv_pos_on_ms, Sequence_List[CMD_sequence_index].lv_pos_off_ms);
	embeddedCliPrint(cli, msg);

	sprintf(msg, "> LV PULSE NEG ON TIME: %dms; LV PULSE NEG OFF TIME: %dms",Sequence_List[CMD_sequence_index].lv_neg_on_ms, Sequence_List[CMD_sequence_index].lv_neg_off_ms);
	embeddedCliPrint(cli, msg);

	if(H_Bridge_State != HB_TASK_IDLE){
		embeddedCliPrint(cli,"> H BRIDGE IS PULSING");
	}
	else if(H_Bridge_State == HB_TASK_IDLE){
		embeddedCliPrint(cli,"> H BRIDGE IS NOT PULSING");
	}

	return;	
}


/*----------------------CMD FOR READ SENSOR-----------------------------*/
static void CMD_GET_SENSOR_GYRO (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_GYRO;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}
static void CMD_GET_SENSOR_ACCEL (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_ACCEL;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_LSM6DSOX (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_LSM6DSOX;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_TEMP (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_TEMP;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_PRESSURE (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_PRESSURE;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_ALTITUDE (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_ALTITUDE;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_BMP390 (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_BMP390;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_H3LIS_VALUE (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_H3LIS331DL;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}
static void CMD_SET_SENSOR_H3LIS_FS (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);

	if (argc < 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_FEW_ARGS");
		return;
	} else if (argc > 1) {
		embeddedCliPrint(cli, "> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm;
	receive_argm = atoi(embeddedCliGetToken(args, 1));

	if((receive_argm != 100) && (receive_argm != 200) && (receive_argm != 400)){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_SET_SENSOR_H3LIS331DL_FS;
	ps_FSP_TX -> Payload.set_sensor_h3lis331dl_fs.fs_value = receive_argm/100;

	fsp_print(2);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_SENSOR_H3LIS_FS (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_H3LIS331DL_FS;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;
}

static void CMD_GET_SENSOR_HV_TEMP (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_HV_TEMP;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
	return;	
}

static void CMD_GET_SENSOR_LV_TEMP (EmbeddedCli *cli, char *args, void *context){
	uint8_t argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"> CMDLINE_INVALID_ARG");
		return;
	}

	ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_LV_TEMP;
	fsp_print(1);

	embeddedCliPrint(cli,"> CMDLINE_OK");
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

static void CMD_SYSTEM_RESET_GP_SW(EmbeddedCli *cli, char *args, void *context) {
	NVIC_SystemReset();
    embeddedCliPrint(cli, "");
}

static void CMD_SYSTEM_RESET_GP_CTRL(EmbeddedCli *cli, char *args, void *context){}
static void CMD_SYSTEM_RESET_GP_ALL(EmbeddedCli *cli, char *args, void *context){}


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
