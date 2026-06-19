/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <cli/CLI_Setup/cli_setup.h>
#include <cli/CLI_Src/embedded_cli.h>
#include <cmd_line_task/xbee_cmd_task.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "cli_command.h"
#include "xbee_cmd_task.h"
#include "uart_driver.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


UART_Driver_t*		CMD_line_handle;
UART_Driver_t  		XBEE_UART;
RingBufElement      g_XBEE_UART_TX_buffer[512];
RingBufElement      g_XBEE_UART_RX_buffer[512];

CLI_UINT            g_XBEE_CMD_line_buffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];
EmbeddedCli 		*p_XBEE_CLI = NULL;



void CMD_Line_Task_Init(void){

	UART_Driver_Init(&XBEE_UART, "DEBUG_UART", XBEE_UART_HANDLE, XBEE_UART_IRQ,
					g_XBEE_UART_TX_buffer, g_XBEE_UART_RX_buffer,
					sizeof(g_XBEE_UART_TX_buffer),sizeof(g_XBEE_UART_RX_buffer));

	p_XBEE_CLI = SystemCLI_Init(&XBEE_UART, g_XBEE_CMD_line_buffer, sizeof(g_XBEE_CMD_line_buffer));

	UART_Driver_SendString(&XBEE_UART, "B");
	UART_Driver_SendString(&XBEE_UART, "\n\r WELCOME GP-3000 \n\r");

	CMD_line_handle = &XBEE_UART;

}

void Debug_CMD_Line_Task(void*){

	char rxData;

	while (UART_Driver_IsDataAvailable(&XBEE_UART))
	{
		rxData = UART_Driver_Read(&XBEE_UART);
		embeddedCliReceiveChar(p_XBEE_CLI, (char)rxData);
	}

	embeddedCliProcess(p_XBEE_CLI);
}



