/*
 * cli_setup.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#define EMBEDDED_CLI_IMPL
#include "cli/CLI_Src/embedded_cli.h"
#include "cli/CLI_Setup/cli_setup.h"
#include "cli_command.h"
#include "uart_driver.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>



static void writeCharToUart_Common(EmbeddedCli *embeddedCli, char c) {

    UART_Driver_t* uart_driver = (UART_Driver_t*)embeddedCli->appContext;
//    uint8_t c_to_send = c;
    UART_Driver_Write(uart_driver, (uint8_t)c);
}


/**
 * Init multil CLI channel
 * @param p_uart: pointer to driver UART handle this channel
 * @param cli_buffer: buffer for this CLI
 * @param  cli_bufferSize: buffer size
 */
EmbeddedCli* SystemCLI_Init(UART_Driver_t* p_uart, CLI_UINT* cli_buffer, uint16_t cli_bufferSize) {

    if (p_uart == NULL || cli_buffer == NULL) return NULL;


    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->cliBuffer = cli_buffer;
    config->cliBufferSize = cli_bufferSize;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    config->maxBindingCount = CLI_MAX_BINDING_COUNT;
    config->enableAutoComplete = CLI_AUTO_COMPLETE;
    config->invitation = CLI_INITATION_CONSOLE;


    config->staticBindings = getCliStaticBindings();
    config->staticBindingCount = getCliStaticBindingCount();


    EmbeddedCli *new_cli = embeddedCliNew(config);

    if (new_cli != NULL) {

        new_cli->appContext = (void*)p_uart;


        new_cli->writeChar = writeCharToUart_Common;
    }

    return new_cli;
}


