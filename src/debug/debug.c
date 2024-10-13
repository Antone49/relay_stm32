#include "debug.h"
#include "lib_log.h"
#include "usart.h"
#include <string.h>

volatile uint16_t debug_uart_data_received;

void debug_data_received_callback(uint16_t size);
void debug_purge();

void debug_init() {
    USART_register_data_received_callback(UART_DEBUG, debug_data_received_callback);
}

void debug_run() {

    if (debug_uart_data_received != 0) {
        char data[200];
        USART_read(UART_DEBUG, data, debug_uart_data_received);
        data[debug_uart_data_received] = '\0';

        bool ret = true;

        LOG_DEBUG("%s", data);

        if (ret == false) {
            LOG_WARN("Debug purge data");
            debug_purge();
        }

        debug_uart_data_received = 0;
    }
}

void debug_purge() {
    debug_uart_data_received = 0;
    USART_purge_data(UART_DEBUG);
}

void debug_data_received_callback(uint16_t size) {
    if (debug_uart_data_received != 0) {
        LOG_WARN("Donnees precentes non traitees, decalage des donnees... erreur"); // ringbuffer ?
    }

    debug_uart_data_received = size;
}