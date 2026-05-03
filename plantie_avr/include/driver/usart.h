#ifndef USART_H
#define USART_H

//polling
void USART_Init(void);
char USART_ReceivePoll(void);
void USART_TransmitPoll(char data);
void USART_TransmitMsgPoll(char* data);

//interrupt
char USART_ReceiveIE(void);
void USART_TransmitIE(char data);
void USART_TransmitMsgIE(char* data);

#endif //USART_H