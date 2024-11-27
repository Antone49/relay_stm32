#include "bluetooth.h"
#include "lib_log.h"
#include "usart.h"
#include <string.h>
#include "relay.h"

#define COMMAND_PUSH_UP        "Up"
#define COMMAND_PUSH_UP_STOP   "UpStop"
#define COMMAND_PUSH_DOWN      "Down"
#define COMMAND_PUSH_DOWN_STOP "DownStop"

volatile uint16_t bluetooth_uart_data_received;

void bluetooth_data_received_callback(uint16_t size);
void bluetooth_purge();

void bluetooth_init() {
    USART_register_data_received_callback(UART_BLUETOOTH, bluetooth_data_received_callback);
}

void bluetooth_run() {

    if (bluetooth_uart_data_received != 0) {
        char data[200];
        USART_read(UART_BLUETOOTH, data, bluetooth_uart_data_received);
        data[bluetooth_uart_data_received] = '\0';

        bool ret = true;

        LOG_DEBUG("%s", data);

        if (strlen(data) == strlen(COMMAND_PUSH_UP) && strcmp(data, COMMAND_PUSH_UP) == 0) {
            relay_push_up();

        } else if (strlen(data) == strlen(COMMAND_PUSH_UP_STOP) && strcmp(data, COMMAND_PUSH_UP_STOP) == 0) {
            relay_push_up_stop();

        } else if (strlen(data) == strlen(COMMAND_PUSH_DOWN) && strcmp(data, COMMAND_PUSH_DOWN) == 0) {
            relay_push_down();

        } else if (strlen(data) == strlen(COMMAND_PUSH_DOWN_STOP) && strcmp(data, COMMAND_PUSH_DOWN_STOP) == 0) {
            relay_push_down_stop();

        } else {
            LOG_WARN("No command found");
            ret = false;
        }

        if (ret == false) {
            LOG_WARN("Debug purge data");
            bluetooth_purge();
        }

        bluetooth_uart_data_received = 0;
    }
}

void bluetooth_purge() {
    bluetooth_uart_data_received = 0;
    USART_purge_data(UART_BLUETOOTH);
}

void bluetooth_data_received_callback(uint16_t size) {
    if (bluetooth_uart_data_received != 0) {
        LOG_WARN("Donnees precentes non traitees, decalage des donnees... erreur"); // ringbuffer ?
    }

    bluetooth_uart_data_received = size;
}