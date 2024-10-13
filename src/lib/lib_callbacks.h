/**
 * @file      lib_callbacks.c
 * @authors   leosticl
 * @date      17/04/2024
 * @copyright 2024 BODET Time&Sport. All rights reserved.
 *
 * @brief
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"

#define CALLBACK_ID_UNDEFINED 0xffff

typedef enum {
    GPIO_EXTI_RISING,
    GPIO_EXTI_FALLING,
    GPIO_EXTI_BOTH,
} callback_gpio_exti_type_e;

/*pointer to a callback function */
typedef void (*callback_FunctionCallback_t)();

void callback_init();
void callback_TIM_IRQHandler();
void callback_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin);
void callback_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);

int callback_timer_register_flag(volatile bool* flag, uint32_t timeout, bool repeat); // Timeout en millisecondes
int callback_timer_register_function(callback_FunctionCallback_t callback, uint32_t timeout, bool repeat); // Timeout en millisecondes
int callback_timer_modify_timeout(int id, uint32_t timeout, bool repeat);
int callback_timer_stop(int id);
void callback_timer_rearmed(int id); // Remise a zero des compteurs

int callback_gpio_exti_register_flag(volatile bool* flag, gpio_id_e gpio, callback_gpio_exti_type_e type, bool repeat);
int callback_gpio_exti_register_function(callback_FunctionCallback_t callback, gpio_id_e gpio, callback_gpio_exti_type_e type, bool repeat);
int callback_gpio_exti_stop(int id);