/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#define UC_GPIO_LIST                                                                                                                                 \
    X(GPIO_RELAY_1, GPIOA, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW)                                                       \
    X(GPIO_RELAY_2, GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW)    

typedef enum {

#define X(id, port, pin, mode, pull, speed) id,
    UC_GPIO_LIST
#undef X

        GPIO_NB_GPIO,
    GPIO_UNUSED
} gpio_id_e;

void GPIO_Init(void);
void GPIO_write(gpio_id_e id, GPIO_PinState state);
void GPIO_toggle(gpio_id_e id);
GPIO_PinState GPIO_read(gpio_id_e id);
uint16_t GPIO_get_pin(gpio_id_e id);
char* GPIO_name(gpio_id_e id);

#endif /*__ GPIO_H__ */
