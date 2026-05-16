#ifndef USART_H
#define USART_H

#include "plantie_io.h"

typedef struct
{
	uint8_t data[128];
	uint8_t size;
} UART_MSG;

void uart_Init(void);

void uart_SendCompleteMsgPoll(IO_PIN uartPin, UART_MSG* inRxMsg);
void uart_GetCompleteRxMsg(IO_PIN uartPin, UART_MSG* inRxMsg);

//polling
char uart_ReceivePoll(IO_PIN uartPin);
void uart_PollEntireMsg(IO_PIN uartPin, UART_MSG* msg);
void uart_TransmitEntireMsg(IO_PIN uartPin, UART_MSG* msg);
void uart_TransmitPoll(IO_PIN uartPin, char data);
void uart_TransmitMsgPoll(IO_PIN uartPin, char* data);
void uart_TransmitTxBinPoll(IO_PIN uartPin, uint8_t* binaryData, uint8_t size);

//interrupt
char uart_ReceiveIE(IO_PIN uartPin);
void uart_TransmitIE(IO_PIN uartPin, char data);
void uart_QueueTxMsgIE(IO_PIN uartPin, UART_MSG* inMsg);
void uart_QueueTxStrIE(IO_PIN uartPin, char* str);
void uart_QueueTxBinIE(IO_PIN uartPin, uint8_t* binaryData, uint8_t size);

void uart_EmptyBufferIE(IO_PIN uartPin);
void uart_EmptyBufferPoll(IO_PIN uartPin);

#endif //USART_H