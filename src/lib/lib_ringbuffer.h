#ifndef _LIB_RING_BUFFER_H__
#define _LIB_RING_BUFFER_H__

#include <stdint.h>

#define RING_BUFFER_LENGTH 1000

typedef struct {
    uint8_t buf[RING_BUFFER_LENGTH];
    uint16_t head, tail;
} RingBuffer_t;

typedef enum {
    RING_BUFFER_OK = 0x0,
    RING_BUFFER_FULL,
    RING_BUFFER_NO_SUFFICIENT_SPACE,
} RingBuffer_status_e;

uint16_t RingBuffer_GetDataLength(RingBuffer_t* buf);
uint16_t RingBuffer_GetFreeSpace(RingBuffer_t* buf);
void RingBuffer_Init(RingBuffer_t* buf);
void RingBuffer_Purge(RingBuffer_t* buf);
uint16_t RingBuffer_Read(RingBuffer_t* buf, uint8_t* data, uint16_t len);
uint8_t RingBuffer_Write(RingBuffer_t* buf, uint8_t* data, uint16_t len);

#endif