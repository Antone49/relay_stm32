#include "relay.h"
#include "lib_log.h"
#include "lib_callbacks.h"
#include <string.h>
#include "hard/gpio.h"

#define PUSH_TIME 500

int relay_1_timer_callback_id = CALLBACK_ID_UNDEFINED;
int relay_2_timer_callback_id = CALLBACK_ID_UNDEFINED;

void relay_1_stop();
void relay_2_stop();

void relay_init() {
    relay_stop();
}

void relay_push_up() {
    LOG_INFO("Relay 1 enabled");

    GPIO_write(GPIO_RELAY_1, GPIO_PIN_SET);

    if (relay_1_timer_callback_id == CALLBACK_ID_UNDEFINED) {
        relay_1_timer_callback_id = callback_timer_register_function(&relay_1_stop, PUSH_TIME, false);
    } else {
        callback_timer_rearmed(relay_1_timer_callback_id);
    }
}

void relay_push_down() {
    LOG_INFO("Relay 2 enabled");

    GPIO_write(GPIO_RELAY_2, GPIO_PIN_SET);

    if (relay_2_timer_callback_id == CALLBACK_ID_UNDEFINED) {
        relay_2_timer_callback_id = callback_timer_register_function(&relay_2_stop, PUSH_TIME, false);
    } else {
        callback_timer_rearmed(relay_2_timer_callback_id);
    }
}

void relay_stop() {
    relay_1_stop();
    relay_2_stop();
}

void relay_1_stop() {
    GPIO_write(GPIO_RELAY_1, GPIO_PIN_RESET);
}

void relay_2_stop() {
    GPIO_write(GPIO_RELAY_2, GPIO_PIN_RESET);
}
