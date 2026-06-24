/*
 * app.h
 *
 *  Created on: Jun 17, 2026
 *      Author: PV
 */

#ifndef APP_H_
#define APP_H_



#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MY_CORE_H_
#include "stm32f4_header.h"

// SYSTEM DRIVER //
#include "board.h"


// USER DRIVER //
#include <scheduler/scheduler.h>
#include "ring_buffer.h"
#include "uart_driver.h"
#include <cli/CLI_Src/embedded_cli.h>
#include <cli/CLI_Setup/cli_setup.h>
#include "cli_command.h"
#include "h_bridge_driver.h"
#include "v_switch_driver.h"
#include "fsp.h"
#include "fsp_frame.h"

// USER DIVICE //

// INCLUDE TASK //
#include "xbee_cmd_task.h"
#include "h_bridge_task.h"
#include "fsp_line_task.h"


typedef enum
{
	CMD_LINE_TASK,
    H_BRIDGE_TASK,

} Task_List_t;

void App_Main(void);




#endif /* APP_H_ */
