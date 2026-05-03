#include "ring_buffer.h"

void ring_buffer_put(ring_buffer* ringBuf, uint8_t data)
{
	ringBuf->dataBuf[ringBuf->head] = data;

	//incrementing the head and checking if we've reached the size limit
	ringBuf->head++;
	if (ringBuf->head == ringBuf->size)
	{
		ringBuf->head = 0;
	}
}

uint8_t ring_buffer_get(ring_buffer* ringBuf)
{
	const uint8_t data = ringBuf->dataBuf[ringBuf->tail];

	ringBuf->tail++;
	if (ringBuf->tail == ringBuf->size)
	{
		ringBuf->tail = 0;
	}

	return data;
}

uint8_t ring_buffer_peek(const ring_buffer* ringBuf)
{
	return ringBuf->dataBuf[ringBuf->tail];
}

bool ring_buffer_isEmpty(const ring_buffer* ringBuf)
{
	return ringBuf->tail == ringBuf->head;
}

bool ring_buffer_isFull(const ring_buffer* ringBuf)
{
	uint8_t indxAfterHead = ringBuf->head + 1;
	if (indxAfterHead == ringBuf->size)
	{
		indxAfterHead = 0;
	}

	return indxAfterHead == ringBuf->tail;
}