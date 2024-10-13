#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "lib_log.h"
#include "usart.h"

int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    uint8_t status = 1;
    if (USART_write(UART_DEBUG, data, len) == false) {
        status = 0;

        // Si on arrive pas a ecrire sur le debug (ringbuffer full), on essaye d'ecrire un caractere pour indiquer un potentiel probleme
        USART_write(UART_DEBUG, ".", 1);
    }

    // return # of bytes written - as best we can tell
    return (status == 1 ? len : 0);
}

int _fstat(int file) {
    return -1;
}

int _close(int file) {
    LOG_ERROR("Function _close not implemented file %d", file);
    return -1;
}

int _isatty(int file) {
    LOG_ERROR("Function _isatty not implemented file %d", file);
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    LOG_ERROR("Function _lseek not implemented file %d", file);
    return 0;
}

int _read(int file, char* ptr, int len) {
    LOG_ERROR("Function _read not implemented file %d", file);
    return 0;
}