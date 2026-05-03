#include "usart.h"
#include "ring_buffer.h"

#include <avr/io.h>
#include "avr/interrupt.h"

#define BAUDERATE_9600    129u
#define BAUDERATE_115200  10u
#define SELECTED_BAUDRATE BAUDERATE_115200

#define USART_BUFFER_SIZE 16
static uint8_t USART0_buffer[USART_BUFFER_SIZE];
static ring_buffer USART0_ringBuffer = {
	.dataBuf = USART0_buffer,
	.size    = USART_BUFFER_SIZE
};

// static uint8_t USART1_buffer[USART_BUFFER_SIZE];
// static ring_buffer USART1_ringBuffer = {
// 	.dataBuf = USART1_buffer,
// 	.size    = USART_BUFFER_SIZE
// };

static inline void USART0_DisableInterruptTX(void)
{
	UCSR0B &= ~(1 << UDRIE0);
}

static inline void USART0_EnableInterruptTX(void)
{
	UCSR0B |= (1 << UDRIE0);
}

//usart0 Rx Complete interrupt
ISR(USART0_RX_vect)
{
	uint8_t data = UDR0;

	ring_buffer_put(&USART0_ringBuffer, data);
}

//usart0 USART0 Data register Empty interrupt
//note, when doing interrupt
ISR(USART0_UDRE_vect)
{
	if (!ring_buffer_isEmpty(&USART0_ringBuffer))
	{
		UDR0 = ring_buffer_get(&USART0_ringBuffer);
	}
	else
	{
		USART0_DisableInterruptTX();
	}
}

// //usart1 rx interrupt
// ISR(USART1_RX_vect)
// {
// }

// //usart1 tx interrupt
// ISR(USART1_TX_vect)
// {
// }

void USART_Init(void)
{
	//Set the baudrate
	//Set the baudrate to 9800 bps using external 20MHz RC Oscillator (see datasheet Table 21-4. Examples of UBRRn Settings for Commonly Used Oscillator Frequencies)
	//UBRR0H and UBRR0L registers are a 16 bit register pair
	///in case the baudrate register value is greater then 255, that means that the value is greater then 8 bits
	//so by bit shifting 8 to the right we remove the first byte and focus on the second byte in the 16 bit value
	UBRR0H = (SELECTED_BAUDRATE >> 8);
	UBRR0L = SELECTED_BAUDRATE; //if the value is greater then 255 then thoose bits will be discarded when setting the low bit register

	//Usart control and status register A
	UCSR0A = 0x00;

	//Enable receiver and transmitter
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);

	//Enable RX complete interrupt
	UCSR0B |= (1 << RXCIE0);

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

char USART_ReceiveIE(void)
{
	if (!ring_buffer_isEmpty(&USART0_ringBuffer))
	{
		return ring_buffer_get(&USART0_ringBuffer);
	}

	return '\0';
}

void USART_TransmitIE(char data)
{
	//wait here until there is enough space to add the next byte
	while (ring_buffer_isFull(&USART0_ringBuffer));

	ring_buffer_put(&USART0_ringBuffer, data);

	//enable interrupt
	USART0_EnableInterruptTX();
}

void USART_TransmitMsgIE(char* data)
{
	int indx = 0;
	while (data[indx] != '\0')
	{
		USART_TransmitIE(data[indx]);
		indx++;
	}
}