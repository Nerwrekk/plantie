#ifndef USART_H
#define USART_H

#include "plantie_io.h"

//polling
void USART_Init(void);
char USART_ReceivePoll(IO_PIN uartPin);
void USART_TransmitPoll(IO_PIN uartPin, char data);
void USART_TransmitMsgPoll(IO_PIN uartPin, char* data);

//interrupt
char USART_ReceiveIE(IO_PIN uartPin);
void USART_TransmitIE(IO_PIN uartPin, char data);
void USART_TransmitMsgIE(IO_PIN uartPin, char* data);

#endif //USART_H