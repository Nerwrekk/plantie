#include "usart.h"
#include "ring_buffer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#define BAUDERATE_9600    129u
#define BAUDERATE_115200  10u
#define SELECTED_BAUDRATE BAUDERATE_115200

#define USART_BUFFER_SIZE 16

//USART0
static uint8_t USART0_TX_buffer[USART_BUFFER_SIZE];
static ring_buffer USART0_TX_ringBuffer = {
	.dataBuf = USART0_TX_buffer,
	.size    = USART_BUFFER_SIZE
};

static uint8_t USART0_RX_buffer[USART_BUFFER_SIZE];
static ring_buffer USART0_RX_ringBuffer = {
	.dataBuf = USART0_RX_buffer,
	.size    = USART_BUFFER_SIZE
};

//USART1
static uint8_t USART1_TX_buffer[USART_BUFFER_SIZE];
static ring_buffer USART1_TX_ringBuffer = {
	.dataBuf = USART1_TX_buffer,
	.size    = USART_BUFFER_SIZE
};

static uint8_t USART1_RX_buffer[USART_BUFFER_SIZE];
static ring_buffer USART1_RX_ringBuffer = {
	.dataBuf = USART1_RX_buffer,
	.size    = USART_BUFFER_SIZE
};

static inline void USART_DisableInterruptTX(IO_PIN pin)
{
	//TODO: ASSERT here
	if (pin == IO_UART_TXD0)
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
	else if (IO_UART_TXD1)
	{
		UCSR1B &= ~(1 << UDRIE1);
	}
}

static inline void USART_EnableInterruptTX(IO_PIN pin)
{
	if (pin == IO_UART_TXD0)
	{
		UCSR0B |= (1 << UDRIE0);
	}
	else if (IO_UART_TXD1)
	{
		UCSR1B |= (1 << UDRIE1);
	}
}

static inline ring_buffer* GetRingBuffer(IO_PIN pin)
{
	switch (pin)
	{
	case IO_UART_RXD0:
		return &USART0_RX_ringBuffer;

	case IO_UART_RXD1:
		return &USART1_RX_ringBuffer;

	case IO_UART_TXD0:
		return &USART0_TX_ringBuffer;

	case IO_UART_TXD1:
		return &USART1_TX_ringBuffer;

	default:
		return NULL;
	}
}

//usart0 Rx Complete interrupt
ISR(USART0_RX_vect)
{
	uint8_t data = UDR0;

	ring_buffer_put(&USART0_RX_ringBuffer, data);
}

//usart0 USART0 Data register Empty interrupt
//note, when doing interrupt
ISR(USART0_UDRE_vect)
{
	if (!ring_buffer_isEmpty(&USART0_TX_ringBuffer))
	{
		UDR0 = ring_buffer_get(&USART0_TX_ringBuffer);
	}
	else
	{
		USART_DisableInterruptTX(IO_UART_TXD0);
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

char USART_ReceivePoll(IO_PIN uartPin)
{
	//Wait for incoming data
	while ((UCSR0A & (1 << RXC0)) == 0);

	return UDR0;
}

void USART_TransmitPoll(IO_PIN uartPin, char data)
{
	//Wait for empty transmit buffer
	while ((UCSR0A & (1 << UDRE0)) == 0);

	//Put data into buffer, sends the data
	UDR0 = data;
}

void USART_TransmitMsgPoll(IO_PIN uartPin, char* data)
{
	int indx = 0;
	while (data[indx] != '\0')
	{
		USART_TransmitPoll(uartPin, data[indx]);
		++indx;
	}
}

char USART_ReceiveIE(IO_PIN uartPin)
{
	if (!ring_buffer_isEmpty(&USART0_TX_ringBuffer))
	{
		return ring_buffer_get(&USART0_TX_ringBuffer);
	}

	return '\0';
}

void USART_TransmitIE(IO_PIN uartPin, char data)
{
	ring_buffer* uartRingBuf = GetRingBuffer(uartPin);

	//wait here until there is enough space to add the next byte
	while (ring_buffer_isFull(uartRingBuf));

	ring_buffer_put(uartRingBuf, data);

	//enable interrupt
	USART_EnableInterruptTX(uartPin);
}

void USART_TransmitMsgIE(IO_PIN uartPin, char* data)
{
	int indx = 0;
	while (data[indx] != '\0')
	{
		USART_TransmitIE(uartPin, data[indx]);
		indx++;
	}
}