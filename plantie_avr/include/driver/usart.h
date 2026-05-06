#ifndef USART_H
#define USART_H

#include "plantie_io.h"

typedef struct
{
	uint8_t data[32];
	uint8_t size;
} RX_MSG;

void USART_Init(void);

void USART_SendCompleteRxMsg(IO_PIN uartPin, RX_MSG* inRxMsg);
void USART_GetCompleteRxMsg(RX_MSG* inRxMsg);

//polling
char USART_ReceivePoll(IO_PIN uartPin);
void USART_TransmitPoll(IO_PIN uartPin, char data);
void USART_TransmitMsgPoll(IO_PIN uartPin, char* data);

//interrupt
char USART_ReceiveIE(IO_PIN uartPin);
void USART_TransmitIE(IO_PIN uartPin, char data);
void USART_TransmitMsgIE(IO_PIN uartPin, char* data);

#endif //USART_H