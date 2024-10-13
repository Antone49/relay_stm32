#include "lib_ringbuffer.h"
#include <string.h>

uint16_t RingBuffer_GetFreeSpace(RingBuffer_t* buf) {
    if (buf->tail == buf->head) {
        return RING_BUFFER_LENGTH - 1;
    }

    if (buf->head > buf->tail) {
        return RING_BUFFER_LENGTH - ((buf->head - buf->tail) + 1);
    } else {
        return (buf->tail - buf->head) - 1;
    }
}

uint16_t RingBuffer_GetDataLength(RingBuffer_t* buf) {
    return RING_BUFFER_LENGTH - (RingBuffer_GetFreeSpace(buf) + 1);
}

void RingBuffer_Init(RingBuffer_t* buf) {
    RingBuffer_Purge(buf);
}

void RingBuffer_Purge(RingBuffer_t* buf) {
    buf->head = buf->tail = 0;
    memset(buf->buf, 0, RING_BUFFER_LENGTH);
}

uint16_t RingBuffer_Read(RingBuffer_t* buf, uint8_t* data, uint16_t len) {
    uint16_t counter = 0;

    while (buf->tail != buf->head && counter < len) {
        data[counter++] = buf->buf[buf->tail];
        buf->tail = (buf->tail + 1) % RING_BUFFER_LENGTH;
    }

    return counter;
}

uint8_t RingBuffer_Write(RingBuffer_t* buf, uint8_t* data, uint16_t len) {
    uint16_t counter = 0;
    uint16_t freeSpace = RingBuffer_GetFreeSpace(buf);

    if (freeSpace == 0) {
        return RING_BUFFER_FULL;
    } else if (freeSpace < len) {
        return RING_BUFFER_NO_SUFFICIENT_SPACE;
    }

    while (counter < len) {
        buf->buf[buf->head] = data[counter++];
        buf->head = (buf->head + 1) % RING_BUFFER_LENGTH;
    }

    return RING_BUFFER_OK;
}