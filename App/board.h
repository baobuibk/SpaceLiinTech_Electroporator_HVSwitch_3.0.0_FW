/*
 * board.h
 *
 *  Created on: Jun 17, 2026
 *      Author: PV
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

/********************** V_BRIDGE **********************/

#define V_SWITCH_EN_PORT		  GPIOB
#define V_SWITCH_EN_PIN			  LL_GPIO_PIN_7

#define V_SWITCH_LIN1_HANDLE    TIM3
#define V_SWITCH_LIN1_CHANNEL   LL_TIM_CHANNEL_CH2

#define V_SWITCH_HIN1_PORT      GPIOB
#define V_SWITCH_HIN1_PIN       LL_GPIO_PIN_6

#define V_SWITCH_LIN2_HANDLE    TIM3
#define V_SWITCH_LIN2_CHANNEL   LL_TIM_CHANNEL_CH1

#define V_SWITCH_HIN2_PORT      GPIOB
#define V_SWITCH_HIN2_PIN       LL_GPIO_PIN_3

#define V_SWITCH_TIM_BASE		TIM2;

/******************************************************/

/********************** H_BRIDGE **********************/

#define H_BRIDGE_HANDLE_PORT  	GPIOC

#define H_BRIDGE_HIN0_PIN       LL_GPIO_PIN_14
#define H_BRIDGE_LIN0_PIN       LL_GPIO_PIN_13

#define H_BRIDGE_HIN1_PIN       LL_GPIO_PIN_0
#define H_BRIDGE_LIN1_PIN       LL_GPIO_PIN_15

#define H_BRIDGE_HIN2_PIN       LL_GPIO_PIN_2
#define H_BRIDGE_LIN2_PIN       LL_GPIO_PIN_1

#define H_BRIDGE_HIN3_PIN       LL_GPIO_PIN_4
#define H_BRIDGE_LIN3_PIN       LL_GPIO_PIN_3

#define H_BRIDGE_HIN4_PIN       LL_GPIO_PIN_6
#define H_BRIDGE_LIN4_PIN       LL_GPIO_PIN_5

#define H_BRIDGE_HIN5_PIN       LL_GPIO_PIN_8
#define H_BRIDGE_LIN5_PIN       LL_GPIO_PIN_7

#define H_BRIDGE_HIN6_PIN       LL_GPIO_PIN_10
#define H_BRIDGE_LIN6_PIN       LL_GPIO_PIN_9

#define H_BRIDGE_HIN7_PIN       LL_GPIO_PIN_12
#define H_BRIDGE_LIN7_PIN       LL_GPIO_PIN_11

#define H_BRIDGE_TIM_HANDLE		TIM1

#define H_BRIDGE_DMA_HANDLE		DMA2
#define H_BRIDGE_DMA_STREAM_HANDLE		LL_DMA_STREAM_5

/******************************************************/

/*********************UART**********************/
#define XBEE_UART_HANDLE       	USART3
#define XBEE_UART_IRQ          	USART3_IRQn

#define GPC_UART_HANDLE			UART4
#define GPC_UART_IRQ			UART4_IRQn

#define GPC_TX_SIZE			    128
#define	GPC_RX_SIZE			    128
#define FSP_BUF_LEN				128
/***********************************************/

/********************* VOM INA229 **********************/
#define VOM_SPI_HANDLE          SPI1
#define VOM_SPI_IRQ             SPI1_IRQn
#define VOM_SPI_DMA_HANDLE      DMA2
#define VOM_SPI_DMA_CHANNEL     LL_DMA_CHANNEL_3
#define VOM_SPI_DMA_TX_STREAM   LL_DMA_STREAM_3 
#define VOM_SPI_DMA_RX_STREAM   LL_DMA_STREAM_0

#define VOM_SPI_CS_PORT         GPIOA
#define VOM_SPI_CS_PIN          LL_GPIO_PIN_4

#define VOM_OVC_PORT            GPIOB
#define VOM_OVC_PIN             LL_GPIO_PIN_0

#define VOM_TIM_HANDLE 			TIM4


/********************* VOM IS66W **********************/

#define IS66W_SPI_HANDLE          SPI2
#define IS66W_SPI_IRQ             SPI2_IRQn

#define IS66W_SPI_DMA_HANDLE      DMA1
#define IS66W_SPI_DMA_CHANNEL     LL_DMA_CHANNEL_0
#define IS66W_SPI_DMA_TX_STREAM   LL_DMA_STREAM_4
#define IS66W_SPI_DMA_RX_STREAM   LL_DMA_STREAM_3

#define IS66W_SPI_CS_PORT         GPIOB
#define IS66W_SPI_CS_PIN          LL_GPIO_PIN_12



#endif /* BOARD_H_ */
