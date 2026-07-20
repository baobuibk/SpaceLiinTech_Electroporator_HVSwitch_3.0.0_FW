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


const CliCommandBinding *getCliStaticBindings(void) ;
uint16_t getCliStaticBindingCount(void);

#endif /* CLI_COMMAND_H_ */
