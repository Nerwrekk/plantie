#ifndef USART_H
#define USART_H

//polling
void USART_Init(void);
char USART_ReceivePoll(void);
void USART_TransmitPoll(char data);
void USART_TransmitMsgPoll(char* data);

//interrupt

#endif //USART_H