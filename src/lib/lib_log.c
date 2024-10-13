#include "lib_log.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "usart.h"

/* Declaration des variables internes ***************************************************************************************/
bool log_stdout_enabled = true;

void log_write(int level, char* format, ...) {
    va_list ap;

    va_start(ap, format);

    if (log_stdout_enabled) {
        fprintf(stdout, "%s ", log_level_name(level));
        vfprintf(stdout, format, ap);
        fprintf(stdout, "\r\n");
    }

    va_end(ap);
}

void log_stdout_set_enable(bool enable) {
    log_stdout_enabled = false;
}

char* log_level_name(log_level_e level) {
    char* ret = "UNKNOWN";

    switch (level) {

#define X(id)                                                                                                                                        \
    case LOG_LEVEL_##id:                                                                                                                             \
        ret = #id;                                                                                                                                   \
        break;

        LIST_LOG_LEVEL
#undef X

        default:
            break;
    }

    return ret;
}
