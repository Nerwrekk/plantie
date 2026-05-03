#include "usart.h"

#include <avr/io.h>

#define BAUDERATE_9600 129u

void USART_Init(void)
{
	//Set the baudrate
	//Set the baudrate to 9800 bps using external 20MHz RC Oscillator (see datasheet Table 21-4. Examples of UBRRn Settings for Commonly Used Oscillator Frequencies)
	//UBRR0H and UBRR0L registers are a 16 bit register pair
	///in case the baudrate register value is greater then 255, that means that the value is greater then 8 bits
	//so by bit shifting 8 to the right we remove the first byte and focus on the second byte in the 16 bit value
	UBRR0H = (BAUDERATE_9600 >> 8);
	UBRR0L = BAUDERATE_9600; //if the value is greater then 255 then thoose bits will be discarded when setting the low bit register

	//Usart control and status register A
	UCSR0A = 0x00;

	//Enable receiver and transmitter
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);

	//USART Mode Selection : Asynchronous
	UCSR0C = 0x6;
}

char USART_ReceivePoll(void)
{
	//Wait for incoming data
	while ((UCSR0A & (1 << RXC0)) == 0);

	return UDR0;
}

void USART_TransmitPoll(char data)
{
	//Wait for empty transmit buffer
	while ((UCSR0A & (1 << UDRE0)) == 0);

	//Put data into buffer, sends the data
	UDR0 = data;
}

void USART_TransmitMsgPoll(char* data)
{
	int indx = 0;
	while (data[indx] != '\0')
	{
		USART_TransmitPoll(data[indx]);
		++indx;
	}
}