/**
 * @file      lib_callbacks.c
 * @authors   leosticl
 * @date      17/04/2024
 * @copyright 2024 BODET Time&Sport. All rights reserved.
 *
 * @brief
 */

#include "lib_callbacks.h"
#include "lib_log.h"
#include "tim.h"
#include "defines.h"

// Nombre defini arbitrairement, on peut les incrementer sans problemes
#define NB_CALLBACK_TIMER     15
#define NB_CALLBACK_GPIO_EXTI 5

typedef struct {
    volatile bool initialized;
    volatile bool* flag;
    callback_FunctionCallback_t callback;
    volatile uint32_t counter;
    uint32_t timeout;
    bool repeat;
    bool running;
} callback_timer_t;

typedef struct {
    volatile bool initialized;
    volatile bool* flag;
    callback_FunctionCallback_t callback;
    uint16_t pin;
    callback_gpio_exti_type_e type;
    bool repeat;
    bool running;
} callback_gpio_exti_t;

callback_timer_t callback_timer_list[NB_CALLBACK_TIMER];
callback_gpio_exti_t callback_gpio_exti_list[NB_CALLBACK_GPIO_EXTI];

void callback_timer_triggered(int id);
void callback_gpio_exti_triggered(int id);

void callback_TIM_IRQHandler() {
    for (size_t i = 0; i < NB_CALLBACK_TIMER; i++) {
        if (callback_timer_list[i].initialized == true && callback_timer_list[i].running == true) {

            callback_timer_list[i].counter--;
            if (callback_timer_list[i].counter == 0) {
                callback_timer_triggered(i);
            }
        }
    }
}

void callback_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
    for (size_t i = 0; i < NB_CALLBACK_GPIO_EXTI; i++) {
        if (callback_gpio_exti_list[i].initialized == true && callback_gpio_exti_list[i].running == true
            && callback_gpio_exti_list[i].pin == GPIO_Pin) {

            if (callback_gpio_exti_list[i].type == GPIO_EXTI_FALLING || callback_gpio_exti_list[i].type == GPIO_EXTI_BOTH) {
                callback_gpio_exti_triggered(i);
            }
        }
    }
}

void callback_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
    for (size_t i = 0; i < NB_CALLBACK_GPIO_EXTI; i++) {
        if (callback_gpio_exti_list[i].initialized == true && callback_gpio_exti_list[i].pin == GPIO_Pin) {

            if (callback_gpio_exti_list[i].type == GPIO_EXTI_RISING || callback_gpio_exti_list[i].type == GPIO_EXTI_BOTH) {
                callback_gpio_exti_triggered(i);
            }
        }
    }
}

void callback_init() {
    for (size_t i = 0; i < NB_CALLBACK_TIMER; i++) {
        callback_timer_list[i].initialized = false;
        callback_timer_list[i].timeout = 0;
        callback_timer_list[i].counter = 0;
        callback_timer_list[i].flag = NULL;
        callback_timer_list[i].callback = NULL;
        callback_timer_list[i].repeat = false;
        callback_timer_list[i].running = false;
    }

    for (size_t i = 0; i < NB_CALLBACK_GPIO_EXTI; i++) {
        callback_gpio_exti_list[i].initialized = false;
        callback_gpio_exti_list[i].flag = NULL;
        callback_gpio_exti_list[i].callback = NULL;
        callback_gpio_exti_list[i].repeat = false;
        callback_gpio_exti_list[i].running = false;
    }
}

int callback_timer_register_flag(volatile bool* flag, uint32_t timeout, bool repeat) {
    if (flag == NULL) {
        LOG_ERROR("Callback timer flag is null");
    }

    int id = CALLBACK_ID_UNDEFINED;
    for (size_t i = 0; i < NB_CALLBACK_TIMER; i++) {
        if (callback_timer_list[i].initialized == false) {
            callback_timer_list[i].initialized = true;
            callback_timer_list[i].timeout = timeout;
            callback_timer_list[i].counter = timeout;
            callback_timer_list[i].flag = flag;
            callback_timer_list[i].repeat = repeat;
            callback_timer_list[i].running = true;

            // Reset flag
            *callback_timer_list[i].flag = false;

            id = i;
            LOG_DEBUG("Callback timer id (%d) register flag, timeout %dms, repeat %s", id, timeout, repeat ? "TRUE" : "FALSE");
            break;
        }
    }

    if (id == CALLBACK_ID_UNDEFINED) {
        LOG_ERROR("Can't register a new callback timer");
    }

    return id;
}

int callback_timer_register_function(callback_FunctionCallback_t callback, uint32_t timeout, bool repeat) {
    if (callback == NULL) {
        LOG_ERROR("Callback timer function is null");
    }

    int id = CALLBACK_ID_UNDEFINED;
    for (size_t i = 0; i < NB_CALLBACK_TIMER; i++) {
        if (callback_timer_list[i].initialized == false) {
            callback_timer_list[i].initialized = true;
            callback_timer_list[i].timeout = timeout;
            callback_timer_list[i].counter = timeout;
            callback_timer_list[i].callback = callback;
            callback_timer_list[i].repeat = repeat;
            callback_timer_list[i].running = true;

            id = i;
            LOG_DEBUG("Callback timer id (%d) register function, timeout %dms, repeat %s", id, timeout, repeat ? "TRUE" : "FALSE");
            break;
        }
    }

    if (id == CALLBACK_ID_UNDEFINED) {
        LOG_ERROR("Can't register a new callback timer");
    }

    return id;
}

int callback_timer_modify_timeout(int id, uint32_t timeout, bool repeat) {
    if (id >= NB_CALLBACK_TIMER) {
        LOG_ERROR("Wrong id (%d) to stop the callback timer", id);
        return CALLBACK_ID_UNDEFINED;
    }

    if (callback_timer_list[id].initialized == false) {
        LOG_ERROR("Callback timer id (%d) is not initialized", id);
        return CALLBACK_ID_UNDEFINED;
    }

    callback_timer_list[id].timeout = timeout;
    callback_timer_list[id].repeat = repeat;
    callback_timer_list[id].running = true;

    // Reset flag
    *callback_timer_list[id].flag = false;

    callback_timer_rearmed(id);

#ifdef CALLBABK_DUMP
    LOG_DEBUG("Callback timer id (%d) new timeout %dms, repeat %s", id, timeout, repeat ? "TRUE" : "FALSE");
#endif

    return id;
}

int callback_timer_stop(int id) {
    if (id >= NB_CALLBACK_TIMER) {
        LOG_ERROR("Wrong id (%d) to stop the callback timer", id);
        return CALLBACK_ID_UNDEFINED;
    }

    if (callback_timer_list[id].initialized == false) {
        LOG_ERROR("Callback timer id (%d) is not initialized", id);
        return CALLBACK_ID_UNDEFINED;
    }

    callback_timer_list[id].initialized = false;
    callback_timer_list[id].timeout = 0;
    callback_timer_list[id].counter = 0;
    callback_timer_list[id].flag = NULL;
    callback_timer_list[id].callback = NULL;
    callback_timer_list[id].repeat = false;
    callback_timer_list[id].running = false;

    LOG_DEBUG("Callback timer id (%d) stopped", id);

    return CALLBACK_ID_UNDEFINED;
}

int callback_gpio_exti_register_flag(volatile bool* flag, gpio_id_e gpio, callback_gpio_exti_type_e type, bool repeat) {
    if (flag == NULL) {
        LOG_ERROR("Callback gpio exti flag is null");
    }

    int id = CALLBACK_ID_UNDEFINED;
    for (size_t i = 0; i < NB_CALLBACK_GPIO_EXTI; i++) {
        if (callback_gpio_exti_list[i].initialized == false) {
            callback_gpio_exti_list[i].initialized = true;
            callback_gpio_exti_list[i].pin = GPIO_get_pin(gpio);
            callback_gpio_exti_list[i].type = type;
            callback_gpio_exti_list[i].flag = flag;
            callback_gpio_exti_list[i].repeat = repeat;
            callback_gpio_exti_list[i].running = true;

            id = i;
            LOG_DEBUG("Callback gpio exti id (%d) register flag, gpio %s, type %d, repeat %s", id, GPIO_name(gpio), type, repeat ? "TRUE" : "FALSE");
            break;
        }
    }

    if (id == CALLBACK_ID_UNDEFINED) {
        LOG_ERROR("Can't register a new callback gpio exti");
    }

    return id;
}

int callback_gpio_exti_register_function(callback_FunctionCallback_t callback, gpio_id_e gpio, callback_gpio_exti_type_e type, bool repeat) {
    if (callback == NULL) {
        LOG_ERROR("Callback gpio exti function is null");
    }

    int id = CALLBACK_ID_UNDEFINED;
    for (size_t i = 0; i < NB_CALLBACK_GPIO_EXTI; i++) {
        if (callback_gpio_exti_list[i].initialized == false) {
            callback_gpio_exti_list[i].initialized = true;
            callback_gpio_exti_list[i].pin = GPIO_get_pin(gpio);
            callback_gpio_exti_list[i].type = type;
            callback_gpio_exti_list[i].callback = callback;
            callback_gpio_exti_list[i].repeat = repeat;
            callback_gpio_exti_list[i].running = true;

            id = i;
            LOG_DEBUG("Callback gpio exti id (%d) register function, gpio %s, type %d, repeat %s", id, GPIO_name(gpio), type,
                      repeat ? "TRUE" : "FALSE");
            break;
        }
    }

    if (id == CALLBACK_ID_UNDEFINED) {
        LOG_ERROR("Can't register a new callback gpio exti");
    }

    return id;
}

int callback_gpio_exti_stop(int id) {
    if (id >= NB_CALLBACK_GPIO_EXTI) {
        LOG_ERROR("Wrong id (%d) to stop the callback gpio exti", id);
        return CALLBACK_ID_UNDEFINED;
    }

    if (callback_gpio_exti_list[id].initialized == false) {
        LOG_ERROR("Callback gpio exti id (%d) is not initialized", id);
        return CALLBACK_ID_UNDEFINED;
    }

    callback_gpio_exti_list[id].initialized = false;
    callback_gpio_exti_list[id].flag = NULL;
    callback_gpio_exti_list[id].callback = NULL;
    callback_gpio_exti_list[id].repeat = false;
    callback_gpio_exti_list[id].running = false;

    LOG_DEBUG("Callback gpio exti id (%d) stopped", id);

    return CALLBACK_ID_UNDEFINED;
}

void callback_timer_rearmed(int id) {
    if (id < NB_CALLBACK_TIMER) {
        if (callback_timer_list[id].initialized) {
            callback_timer_list[id].counter = callback_timer_list[id].timeout;
            callback_timer_list[id].running = true;
        } else {
            LOG_WARN("Callback timer id not initialized");
        }
    } else {
        LOG_ERROR("Callback timer incorrect id %d >= %d", id, NB_CALLBACK_TIMER);
    }
}

/* Private function */
void callback_timer_triggered(int id) {
    if (callback_timer_list[id].flag != NULL) {
        *callback_timer_list[id].flag = true;
    }

    if (callback_timer_list[id].callback != NULL) {
        (*(callback_timer_list[id].callback))();
    }

    // Rearmed or stop the callback
    if (callback_timer_list[id].repeat == true) {
        callback_timer_list[id].counter = callback_timer_list[id].timeout;
    } else {
        callback_timer_list[id].running = false;
    }
}

void callback_gpio_exti_triggered(int id) {
    if (callback_gpio_exti_list[id].flag != NULL) {
        *callback_gpio_exti_list[id].flag = true;
    }

    if (callback_gpio_exti_list[id].callback != NULL) {
        (*(callback_gpio_exti_list[id].callback))();
    }

    // Stop the callback if repeat is false
    if (callback_gpio_exti_list[id].repeat == false) {
        callback_gpio_exti_list[id].running = false;
    }
}