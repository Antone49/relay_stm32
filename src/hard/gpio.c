/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"
#include "lib_log.h"
#include "main.h"

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/

/** Configure pins
*/
void GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

#define X(id, port, pin, mode, pull, speed)                                                                                                          \
    /*Configure GPIO pin Output Level */                                                                                                             \
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);                                                                                                    \
                                                                                                                                                     \
    /*Configure GPIO pins */                                                                                                                         \
    GPIO_InitStruct.Pin = pin;                                                                                                                       \
    GPIO_InitStruct.Mode = mode;                                                                                                                     \
    GPIO_InitStruct.Pull = pull;                                                                                                                     \
    GPIO_InitStruct.Speed = speed;                                                                                                                   \
    HAL_GPIO_Init(port, &GPIO_InitStruct);

    UC_GPIO_LIST
#undef X

}

void GPIO_write(gpio_id_e id, GPIO_PinState state) {

    switch (id) {

#define X(idx, port, pin, mode, pull, speed)                                                                                                         \
    case idx:                                                                                                                                        \
        HAL_GPIO_WritePin(port, pin, state);                                                                                                         \
        break;

        UC_GPIO_LIST
#undef X

        default:
            LOG_ERROR("GPIO_write: id (%d) not found", id);
            break;
    }
}

void GPIO_toggle(gpio_id_e id) {
    switch (id) {

#define X(idx, port, pin, mode, pull, speed)                                                                                                         \
    case idx:                                                                                                                                        \
        HAL_GPIO_TogglePin(port, pin);                                                                                                               \
        break;

        UC_GPIO_LIST
#undef X

        default:
            LOG_ERROR("GPIO_toggle: id (%d) not found", id);
            break;
    }
}

GPIO_PinState GPIO_read(gpio_id_e id) {
    GPIO_PinState status = GPIO_PIN_RESET;

    switch (id) {

#define X(idx, port, pin, mode, pull, speed)                                                                                                         \
    case idx:                                                                                                                                        \
        status = HAL_GPIO_ReadPin(port, pin);                                                                                                        \
        break;

        UC_GPIO_LIST
#undef X

        default:
            LOG_ERROR("GPIO_read: id (%d) not found", id);
            break;
    }

    return status;
}

char* GPIO_name(gpio_id_e id) {
    char* ret = "UNKNOWN";

    switch (id) {

#define X(idx, port, pin, mode, pull, speed)                                                                                                         \
    case idx:                                                                                                                                        \
        ret = #idx;                                                                                                                                  \
        break;

        UC_GPIO_LIST
#undef X

        default:
            LOG_ERROR("GPIO_name: id (%d) not found", id);
            break;
    }

    return ret;
}

uint16_t GPIO_get_pin(gpio_id_e id) {
    uint16_t ret = 0;

    switch (id) {

#define X(idx, port, pin, mode, pull, speed)                                                                                                         \
    case idx:                                                                                                                                        \
        ret = pin;                                                                                                                                   \
        break;

        UC_GPIO_LIST
#undef X

        default:
            LOG_ERROR("GPIO_get_pin: id (%d) not found", id);
            break;
    }

    return ret;
}