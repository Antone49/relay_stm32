/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usart.h"
#include "lib_log.h"
#include "lib_ringbuffer.h"
#include "gpio.h"

#define USART_TX_WRITE_BUFFER_SIZE 200

typedef struct {
    UART_HandleTypeDef huart;
    int uartInterrupt;
    DMA_HandleTypeDef TxDmaHandle;
    int txDmaInterrupt;
    RingBuffer_t TxRingbuffer;
    uint8_t TxWriteBuffer[USART_TX_WRITE_BUFFER_SIZE];
    DMA_HandleTypeDef RxDmaHandle;
    int rxDmaInterrupt;
    uint8_t RxRingbuffer[USART_RX_READ_BUFFER_SIZE];
    uint16_t RxRingbufferTail;
    uint16_t RxRingbufferHeadIdle;
    USART_RxDataReceivedCallback_t RxDataReceivedCallback;
} uart_t;

uart_t uart_handle[NB_UART];

void USART_USART1_Init(void);
bool USART_tx_write(uart_e uart);

void USART_Init(void) {

    for (size_t i = 0; i < NB_UART; i++) {
        RingBuffer_Init(&uart_handle[i].TxRingbuffer);
        uart_handle[i].RxRingbufferTail = 0;
        uart_handle[i].RxRingbufferHeadIdle = 0;
        uart_handle[i].RxDataReceivedCallback = NULL;
    }

    USART_USART1_Init();
}

/* USART1 init function */
void USART_USART1_Init(void) {
    uart_handle[UART_DEBUG].huart.Instance = USART1;
    uart_handle[UART_DEBUG].huart.Init.BaudRate = 115200;
    uart_handle[UART_DEBUG].huart.Init.WordLength = UART_WORDLENGTH_8B;
    uart_handle[UART_DEBUG].huart.Init.StopBits = UART_STOPBITS_1;
    uart_handle[UART_DEBUG].huart.Init.Parity = UART_PARITY_NONE;
    uart_handle[UART_DEBUG].huart.Init.Mode = UART_MODE_TX_RX;
    uart_handle[UART_DEBUG].huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart_handle[UART_DEBUG].huart.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&uart_handle[UART_DEBUG].huart) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_UARTEx_ReceiveToIdle_DMA(&uart_handle[UART_DEBUG].huart, uart_handle[UART_DEBUG].RxRingbuffer, USART_RX_READ_BUFFER_SIZE) != HAL_OK) {
        LOG_ERROR("HAL_UARTEx_ReceiveToIdle_DMA");
    }

    // Disable interruption half transfer and tranfer complete which call HAL_UARTEx_RxEventCallback and cut the frame
    __HAL_DMA_DISABLE_IT(&uart_handle[UART_DEBUG].RxDmaHandle, DMA_IT_TC);
    __HAL_DMA_DISABLE_IT(&uart_handle[UART_DEBUG].RxDmaHandle, DMA_IT_HT);
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (uartHandle->Instance == USART1) {
        LOG_INFO("HAL_UART_MspDeInit");

        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = UC_UART_DEBUG_TX_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(UC_UART_DEBUG_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = UC_UART_DEBUG_RX_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(UC_UART_DEBUG_GPIO_Port, &GPIO_InitStruct);

        /* USART DMA TX Init */
        uart_handle[UART_DEBUG].TxDmaHandle.Instance = DMA1_Channel4;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
        uart_handle[UART_DEBUG].TxDmaHandle.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&uart_handle[UART_DEBUG].TxDmaHandle) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmatx, uart_handle[UART_DEBUG].TxDmaHandle);

        /* USART DMA RX Init */
        uart_handle[UART_DEBUG].RxDmaHandle.Instance = DMA1_Channel5;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
        uart_handle[UART_DEBUG].RxDmaHandle.Init.Mode = DMA_CIRCULAR;
        if (HAL_DMA_Init(&uart_handle[UART_DEBUG].RxDmaHandle) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmarx, uart_handle[UART_DEBUG].RxDmaHandle);

        /* USART interrupt Init */
        uart_handle[UART_DEBUG].uartInterrupt = USART1_IRQn;
        HAL_NVIC_SetPriority(uart_handle[UART_DEBUG].uartInterrupt, 0, 0);
        HAL_NVIC_EnableIRQ(uart_handle[UART_DEBUG].uartInterrupt);

        /* DMA TX interrupt Init */
        uart_handle[UART_DEBUG].txDmaInterrupt = DMA1_Channel5_IRQn;
        HAL_NVIC_SetPriority(uart_handle[UART_DEBUG].txDmaInterrupt, 0, 0);
        HAL_NVIC_EnableIRQ(uart_handle[UART_DEBUG].txDmaInterrupt);

        /* DMA RX interrupt Init */
        uart_handle[UART_DEBUG].rxDmaInterrupt = DMA1_Channel4_IRQn;
        HAL_NVIC_SetPriority(uart_handle[UART_DEBUG].rxDmaInterrupt, 0, 0);
        HAL_NVIC_EnableIRQ(uart_handle[UART_DEBUG].rxDmaInterrupt);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle) {
    LOG_INFO("HAL_UART_MspDeInit");

    if (uartHandle->Instance == USART1) {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        HAL_GPIO_DeInit(UC_UART_DEBUG_GPIO_Port, UC_UART_DEBUG_TX_Pin | UC_UART_DEBUG_RX_Pin);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}

DMA_HandleTypeDef* USART_tx_dma_handle(uart_e uart) {
    return &uart_handle[uart].TxDmaHandle;
}

DMA_HandleTypeDef* USART_rx_dma_handle(uart_e uart) {
    return &uart_handle[uart].RxDmaHandle;
}

UART_HandleTypeDef* USART_uart_handle(uart_e uart) {
    return &uart_handle[uart].huart;
}

char* USART_device_name(uart_e uart) {
    char* ret = "UNKNOWN";

    switch (uart) {

#define X(id)                                                                                                                                        \
    case id:                                                                                                                                         \
        ret = #id;                                                                                                                                   \
        break;

        LIST_UART
#undef X

        default:
            break;
    }

    return ret;
}

void USART_purge_data(uart_e uart) {
    LOG_WARN("USART_purge on %s", USART_device_name(uart));

    RingBuffer_Purge(&uart_handle[uart].TxRingbuffer);
    uart_handle[uart].RxRingbufferTail = uart_handle[uart].RxRingbufferHeadIdle;
}

/*****************************************/
/*              TX function              */
/*****************************************/
bool USART_write(uart_e uart, char* data, uint16_t size) {
    bool ret = true;

    if (uart < NB_UART) {
        HAL_NVIC_DisableIRQ(uart_handle[uart].uartInterrupt);
        HAL_NVIC_DisableIRQ(uart_handle[uart].txDmaInterrupt);

        // Ajout donnees au ring buffer
        if (RingBuffer_Write(&uart_handle[uart].TxRingbuffer, (uint8_t*)(data), size) == RING_BUFFER_OK) {
            if (HAL_DMA_GetState(&uart_handle[uart].TxDmaHandle) == HAL_DMA_STATE_READY) {
                ret = USART_tx_write(uart);
            }
        } else {
            if (uart != UART_DEBUG) {
                // RingBuffer full ?
                LOG_ERROR("Can't write on %s (size: %d)", USART_device_name(uart), size);
            }

            ret = false;
        }

        HAL_NVIC_EnableIRQ(uart_handle[uart].uartInterrupt);
        HAL_NVIC_EnableIRQ(uart_handle[uart].txDmaInterrupt);
    } else {
        ret = false;
        LOG_ERROR("Uart doesn't exist %d", uart);
    }

    return ret;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* UartHandle) {
    for (size_t i = 0; i < NB_UART; i++) {
        if (&uart_handle[i].huart == UartHandle) {
            HAL_NVIC_DisableIRQ(uart_handle[i].uartInterrupt);
            HAL_NVIC_DisableIRQ(uart_handle[i].txDmaInterrupt);

            USART_tx_write(i);

            HAL_NVIC_EnableIRQ(uart_handle[i].uartInterrupt);
            HAL_NVIC_EnableIRQ(uart_handle[i].txDmaInterrupt);

            break;
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    for (size_t i = 0; i < NB_UART; i++) {
        if (&uart_handle[i].huart == huart) {

            // Probleme observe quand on a des collisions sur la ligne UART, le DMA passe en etat Abort
            // Reproduction, le scorepad emet les donnees Varsys et avec un espion sur la ligne entre les deux. On emet egalement des donnees finit par creer cette erreur (frame error, noise error)
            // L'etat du DMA est repasse a ready par l'interruption HAL_DMA_IRQHandler qui appelle cette fonction
            if (uart_handle[i].RxDmaHandle.State == HAL_DMA_STATE_READY) {

                // Relance l'ecoute sur l'UART
                uart_handle[i].RxRingbufferTail = uart_handle[i].RxRingbufferHeadIdle = 0;
                HAL_UARTEx_ReceiveToIdle_DMA(&uart_handle[i].huart, uart_handle[i].RxRingbuffer, USART_RX_READ_BUFFER_SIZE);

                // Disable interruption half transfer and tranfer complete which call HAL_UARTEx_RxEventCallback and cut the frame
                __HAL_DMA_DISABLE_IT(&uart_handle[i].RxDmaHandle, DMA_IT_TC);
                __HAL_DMA_DISABLE_IT(&uart_handle[i].RxDmaHandle, DMA_IT_HT);
            } else {
                LOG_ERROR("HAL_UART_ErrorCallback on %s, errorCode 0x%X", USART_device_name(i), huart->ErrorCode);
            }
            break;
        }
    }
}

bool USART_tx_write(uart_e uart) {
    bool ret = true;

    int size = RingBuffer_GetDataLength(&uart_handle[uart].TxRingbuffer);
    if (size != 0) {
        if (size > USART_TX_WRITE_BUFFER_SIZE) {
            size = USART_TX_WRITE_BUFFER_SIZE;
        }

        RingBuffer_Read(&uart_handle[uart].TxRingbuffer, uart_handle[uart].TxWriteBuffer, size);
        if (HAL_UART_Transmit_DMA(&uart_handle[uart].huart, uart_handle[uart].TxWriteBuffer, size) != HAL_OK) {
            ret = false;

            // Boucle infini, si une erreur sur l'uart DEBUG et qu'on essaye d'ecrire l'erreur sur l'uart DEBUG
            if (uart != UART_DEBUG) {
                LOG_WARN("Can't send data on %s (size: %d)", USART_device_name(uart), size);
            }
        }
    }

    return ret;
}

/*****************************************/
/*              RX function              */
/*****************************************/
uint16_t USART_read_nb_data_available(uart_e uart) {
    uint16_t counter = 0;
    uint16_t head = USART_RX_READ_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&uart_handle[uart].RxDmaHandle);

    if (head >= uart_handle[uart].RxRingbufferTail) {
        counter = head - uart_handle[uart].RxRingbufferTail;
    } else {
        counter = USART_RX_READ_BUFFER_SIZE + head - uart_handle[uart].RxRingbufferTail;
    }

    return counter;
}

void USART_register_data_received_callback(uart_e uart, USART_RxDataReceivedCallback_t callback) {
    if (uart < NB_UART) {
        uart_handle[uart].RxDataReceivedCallback = callback;
    } else {
        LOG_ERROR("Uart doesn't exist %d", uart);
    }
}

uint16_t USART_read(uart_e uart, char* data, uint16_t size) {
    uint16_t counter = 0;
    uint16_t head = USART_RX_READ_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&uart_handle[uart].RxDmaHandle);

    while (uart_handle[uart].RxRingbufferTail != head && counter < size) {
        data[counter++] = uart_handle[uart].RxRingbuffer[uart_handle[uart].RxRingbufferTail];
        uart_handle[uart].RxRingbufferTail = (uart_handle[uart].RxRingbufferTail + 1) % USART_RX_READ_BUFFER_SIZE;
    }

    return counter;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t pos) {
    for (size_t i = 0; i < NB_UART; i++) {
        if (&uart_handle[i].huart == huart) {

            // Calcul de la taille reel depuis le dernier idle
            uint16_t realSize;
            if (pos >= uart_handle[i].RxRingbufferHeadIdle) {
                realSize = pos - uart_handle[i].RxRingbufferHeadIdle;
            } else {
                realSize = USART_RX_READ_BUFFER_SIZE + pos - uart_handle[i].RxRingbufferHeadIdle;
            }

            uart_handle[i].RxRingbufferHeadIdle = pos;

            if (uart_handle[i].RxDataReceivedCallback != NULL) {
                (*(uart_handle[i].RxDataReceivedCallback))(realSize);
            }

            break;
        }
    }
}