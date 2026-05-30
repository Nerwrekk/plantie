#include "usart.h"
#include "ring_buffer.h"
#include "plantie_globals.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#define BAUDERATE_9600_8MZH 51u
#define BAUDERATE_9600      129u
#define BAUDERATE_115200    10u
#define SELECTED_BAUDRATE   BAUDERATE_9600_8MZH

#define USART0_BUFFER_SIZE 128
#define USART1_BUFFER_SIZE 128

//USART0
static uint8_t USART0_TX_buffer[USART0_BUFFER_SIZE];
static ring_buffer USART0_TX_ringBuffer = {
	.dataBuf = USART0_TX_buffer,
	.size    = USART0_BUFFER_SIZE
};

static uint8_t USART0_RX_buffer[USART0_BUFFER_SIZE];
static ring_buffer USART0_RX_ringBuffer = {
	.dataBuf = USART0_RX_buffer,
	.size    = USART0_BUFFER_SIZE
};

//USART1
static uint8_t USART1_TX_buffer[USART1_BUFFER_SIZE];
static ring_buffer USART1_TX_ringBuffer = {
	.dataBuf = USART1_TX_buffer,
	.size    = USART1_BUFFER_SIZE
};

static uint8_t USART1_RX_buffer[USART1_BUFFER_SIZE];
static ring_buffer USART1_RX_ringBuffer = {
	.dataBuf = USART1_RX_buffer,
	.size    = USART1_BUFFER_SIZE
};

static inline void uart_DisableInterruptTX(IO_PIN pin)
{
	//TODO: ASSERT here
	if (pin == IO_UART_PC_TX)
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
	else if (IO_UART_ESP_TX)
	{
		UCSR1B &= ~(1 << UDRIE1);
	}
}

static inline void uart_EnableInterruptTX(IO_PIN pin)
{
	if (pin == IO_UART_PC_TX)
	{
		UCSR0B |= (1 << UDRIE0);
	}
	else if (IO_UART_ESP_TX)
	{
		UCSR1B |= (1 << UDRIE1);
	}
}

static inline ring_buffer* GetRingBuffer(IO_PIN pin)
{
	switch (pin)
	{
	case IO_UART_PC_RX:
		return &USART0_RX_ringBuffer;

	case IO_UART_ESP_RX:
		return &USART1_RX_ringBuffer;

	case IO_UART_PC_TX:
		return &USART0_TX_ringBuffer;

	case IO_UART_ESP_TX:
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

	if (data == '\n' || data == '\0')
	{
		PLANTIE_FLAGS |= PC_RX_MSG_RDY;
	}
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
		uart_DisableInterruptTX(IO_UART_PC_TX);
	}
}

//usart1 Rx Complete interrupt
ISR(USART1_RX_vect)
{
	uint8_t data = UDR1;

	ring_buffer_put(&USART1_RX_ringBuffer, data);

	if (data == '>' || data == '\n' || data == '\0')
	{
		PLANTIE_FLAGS |= ESP_RX_MSG_RDY;
	}
}

//usart1 tx interrupt
ISR(USART1_UDRE_vect)
{
	if (!ring_buffer_isEmpty(&USART1_TX_ringBuffer))
	{
		UDR1 = ring_buffer_get(&USART1_TX_ringBuffer);
	}
	else
	{
		uart_DisableInterruptTX(IO_UART_ESP_TX);
	}
}

void uart_Init(void)
{
	//Set the baudrate
	//Set the baudrate to 9800 bps using external 20MHz RC Oscillator (see datasheet Table 21-4. Examples of UBRRn Settings for Commonly Used Oscillator Frequencies)
	//UBRR0H and UBRR0L registers are a 16 bit register pair
	///in case the baudrate register value is greater then 255, that means that the value is greater then 8 bits
	//so by bit shifting 8 to the right we remove the first byte and focus on the second byte in the 16 bit value
	UBRR0H = (SELECTED_BAUDRATE >> 8);
	UBRR0L = SELECTED_BAUDRATE; //if the value is greater then 255 then thoose bits will be discarded when setting the low bit register

	UBRR1H = (SELECTED_BAUDRATE >> 8);
	UBRR1L = SELECTED_BAUDRATE; //if the value is greater then 255 then thoose bits will be discarded when setting the low bit register

	//Usart control and status register A
	UCSR0A = 0x00;

	UCSR1A = 0x00;

	//Enable receiver and transmitter
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);

	UCSR1B |= (1 << RXEN1);
	UCSR1B |= (1 << TXEN1);

	//Enable RX complete interrupt
	UCSR0B |= (1 << RXCIE0);
	UCSR1B |= (1 << RXCIE1);

	//USART Mode Selection : Asynchronous
	UCSR0C = 0x6;
	UCSR1C = 0x6;
}

void uart_SendCompleteMsgPoll(IO_PIN uartPin, UART_MSG* inRxMsg)
{
	for (uint8_t i = 0; i < inRxMsg->size; i++)
	{
		uart_TransmitPoll(uartPin, (char)inRxMsg->data[i]);
	}
}

void uart_GetCompleteRxMsg(IO_PIN uartPin, UART_MSG* inRxMsg)
{
	ring_buffer* ringBuf = GetRingBuffer(uartPin);
	uint8_t indx         = 0;
	while (true)
	{
		if (ring_buffer_isEmpty(ringBuf))
		{
			break;
		}

		uint8_t byte        = ring_buffer_get(ringBuf);
		inRxMsg->data[indx] = byte;
		inRxMsg->size++;
		indx++;

		if (byte == '\0')
		{
			break;
		}
	}
}

char uart_ReceivePoll(IO_PIN uartPin)
{
	switch (uartPin)
	{
	case IO_UART_PC_RX:
	{
		//Wait for incoming data
		while ((UCSR0A & (1 << RXC0)) == 0);

		return UDR0;
	}
	case IO_UART_ESP_RX:
	{
		//Wait for incoming data
		while ((UCSR1A & (1 << RXC1)) == 0);

		return UDR1;
	}
	default:
		return '\0';
	}
}

void uart_TransmitPoll(IO_PIN uartPin, char data)
{
	switch (uartPin)
	{
	case IO_UART_PC_TX:
	{
		//Wait for empty transmit buffer
		while ((UCSR0A & (1 << UDRE0)) == 0);

		//Put data into buffer, sends the data
		UDR0 = data;

		break;
	}
	case IO_UART_ESP_TX:
	{
		//Wait for empty transmit buffer
		while ((UCSR1A & (1 << UDRE1)) == 0);

		//Put data into buffer, sends the data
		UDR1 = data;

		break;
	}
	default:
		return;
	}
}

void uart_TransmitMsgPoll(IO_PIN uartPin, char* data)
{
	int indx = 0;
	while (data[indx] != '\0')
	{
		uart_TransmitPoll(uartPin, data[indx]);
		indx++;
	}
}

void uart_TransmitTxBinPoll(IO_PIN uartPin, uint8_t* binaryData, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		uart_TransmitPoll(uartPin, binaryData[i]);
	}
}

char uart_ReceiveIE(IO_PIN uartPin)
{
	if (!ring_buffer_isEmpty(&USART0_TX_ringBuffer))
	{
		return ring_buffer_get(&USART0_TX_ringBuffer);
	}

	return '\0';
}

void uart_TransmitIE(IO_PIN uartPin, char data)
{
	ring_buffer* uartRingBuf = GetRingBuffer(uartPin);

	//wait here until there is enough space to add the next byte
	while (ring_buffer_isFull(uartRingBuf));

	ring_buffer_put(uartRingBuf, data);

	//enable interrupt
	uart_EnableInterruptTX(uartPin);
}

void uart_QueueTxMsgIE(IO_PIN uartPin, UART_MSG* inMsg)
{
	ring_buffer* ringBuf = GetRingBuffer(uartPin);

	for (uint8_t i = 0; i < inMsg->size; i++)
	{
		ring_buffer_put(ringBuf, inMsg->data[i]);
	}

	uart_EnableInterruptTX(uartPin);
}

void uart_QueueTxStrIE(IO_PIN uartPin, char* str)
{
	ring_buffer* ringBuf = GetRingBuffer(uartPin);

	uint8_t indx = 0;
	while (str[indx] != '\0')
	{
		ring_buffer_put(ringBuf, str[indx]);
		indx++;
	}

	uart_EnableInterruptTX(uartPin);
}

void uart_QueueTxBinIE(IO_PIN uartPin, uint8_t* binaryData, uint8_t size)
{
	ring_buffer* ringBuf = GetRingBuffer(uartPin);

	for (uint8_t i = 0; i < size; i++)
	{
		ring_buffer_put(ringBuf, binaryData[i]);
	}

	uart_EnableInterruptTX(uartPin);
}

void uart_EmptyBufferIE(IO_PIN uartPin)
{
	uint8_t sreg = SREG;
	cli();

	ring_buffer* ringBuf = GetRingBuffer(uartPin);

	while (!ring_buffer_isEmpty(ringBuf))
	{
		ring_buffer_get(ringBuf);
	}

	SREG = sreg;
}
static volatile uint8_t dummy = 0;
void uart_EmptyBufferPoll(IO_PIN uartPin)
{
	if (uartPin == IO_UART_ESP_RX)
	{
		while (UCSR1A & (1 << RXC1))
		{
			dummy = UDR1;
		}
	}
	else
	{
		while (UCSR0A & (1 << RXC0))
		{
			dummy = UDR0;
		}
	}
}

void uart_PollEntireMsg(IO_PIN uartPin, UART_MSG* msg)
{
	if (uartPin == IO_UART_ESP_RX)
	{
		char data    = '\0';
		uint8_t indx = 0;
		do
		{
			//Wait for empty transmit buffer
			while ((UCSR1A & (1 << RXC1)) == 0);

			//Put data into buffer, sends the data
			data            = UDR1;
			msg->data[indx] = data;
			msg->size++;
			indx++;

		} while (data != '\n' && data != '>' && indx < 60);
	}
	else
	{
		char data    = '\0';
		uint8_t indx = 0;
		do
		{
			while ((UCSR0A & (1 << RXC0)) == 0);

			//Put data into buffer, sends the data
			data            = UDR0;
			msg->data[indx] = data;
			msg->size++;
			indx++;

		} while (data != '\n' && data != '\0' && indx < 60);
	}
}

void uart_TransmitEntireMsg(IO_PIN uartPin, UART_MSG* msg)
{
	for (uint8_t i = 0; i < msg->size; i++)
	{
		uart_TransmitPoll(uartPin, msg->data[i]);
	}
}
