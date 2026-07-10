/*
 * cli_command.h
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#ifndef CLI_COMMAND_H_
#define CLI_COMMAND_H_

#include <cli/CLI_Setup/cli_setup.h>
#include <cli/CLI_Src/embedded_cli.h>
#include "main.h"

#include <stdio.h>

extern uint8_t CMD_sequence_index ;



// extern uint16_t hv_set_volt ;
// extern uint16_t lv_set_volt ;

// extern uint16_t hv_raw_volt;
// extern uint16_t lv_raw_volt;

// extern bool hv_is_charging;
// extern bool lv_is_charging;

// extern bool hv_is_discharging;
// extern bool lv_is_discharging;

const CliCommandBinding *getCliStaticBindings(void) ;
uint16_t getCliStaticBindingCount(void);

#endif /* CLI_COMMAND_H_ */
