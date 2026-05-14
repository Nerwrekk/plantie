#ifndef USART_H
#define USART_H

#include "plantie_io.h"

typedef struct
{
	uint8_t data[64];
	uint8_t size;
} UART_MSG;

void uart_Init(void);

void uart_SendCompleteMsgPoll(IO_PIN uartPin, UART_MSG* inRxMsg);
void uart_GetCompleteRxMsg(IO_PIN uartPin, UART_MSG* inRxMsg);

//polling
char uart_ReceivePoll(IO_PIN uartPin);
void uart_TransmitPoll(IO_PIN uartPin, char data);
void uart_TransmitMsgPoll(IO_PIN uartPin, char* data);

//interrupt
char uart_ReceiveIE(IO_PIN uartPin);
void uart_TransmitIE(IO_PIN uartPin, char data);
void uart_QueueTxMsgIE(IO_PIN uartPin, UART_MSG* inMsg);
void uart_QueueTxStrIE(IO_PIN uartPin, char* str);

#endif //USART_H