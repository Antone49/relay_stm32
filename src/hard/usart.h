/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>

#define UC_UART_DEBUG_TX_Pin    GPIO_PIN_9
#define UC_UART_DEBUG_RX_Pin    GPIO_PIN_10
#define UC_UART_DEBUG_GPIO_Port GPIOA

#define LIST_UART               X(UART_DEBUG)

#define X(name)                 name,

typedef enum {
    LIST_UART NB_UART,
} uart_e;

#undef X

#define USART_RX_READ_BUFFER_SIZE 1000

/*pointer to a UART Rx Event specific callback function */
typedef void (*USART_RxDataReceivedCallback_t)(uint16_t size);

void USART_Init(void);
bool USART_write(uart_e uart, char* data, uint16_t size);
uint16_t USART_read_nb_data_available(uart_e uart);
uint16_t USART_read(uart_e uart, char* data, uint16_t size);
void USART_purge_data(uart_e uart);
void USART_register_data_received_callback(uart_e uart, USART_RxDataReceivedCallback_t callback);
char* USART_device_name(uart_e uart);

UART_HandleTypeDef* USART_uart_handle(uart_e uart);
DMA_HandleTypeDef* USART_tx_dma_handle(uart_e uart);
DMA_HandleTypeDef* USART_rx_dma_handle(uart_e uart);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
