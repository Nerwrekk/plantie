#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	uint8_t* dataBuf;
	uint8_t size;
	uint8_t head;
	uint8_t tail;
} ring_buffer;

void ring_buffer_put(ring_buffer* ringBuf, uint8_t data);
uint8_t ring_buffer_get(ring_buffer* ringBuf);
uint8_t ring_buffer_peek(const ring_buffer* ringBuf);
bool ring_buffer_isEmpty(const ring_buffer* ringBuf);
bool ring_buffer_isFull(const ring_buffer* ringBuf);

#endif //RING_BUFFER_H