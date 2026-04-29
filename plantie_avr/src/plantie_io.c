#include "plantie_io.h"

#include <avr/io.h>

#define IO_PORT_CNT (4u)

#define IO_PORT_OFFSET (3u)
#define IO_PORT_MASK   (3u << IO_PORT_OFFSET)
#define IO_PIN_MASK    (0x7u)

static volatile uint8_t* const portDirRegisters[IO_PORT_CNT] = { &DDRA, &DDRB, &DDRC, &DDRD };
static volatile uint8_t* const portOutRegisters[IO_PORT_CNT] = { &PORTA, &PORTB, &PORTC, &PORTD };

static inline uint8_t IO_GetPort(io_pin_e pin)
{
	return (pin & IO_PORT_MASK) >> IO_PORT_OFFSET;
}

static inline uint8_t IO_GetPinIndex(io_pin_e pin)
{
	return (pin & IO_PIN_MASK); //NOTE to self, always double check that you are using the right define!!
}

static inline uint8_t IO_GetPinBit(io_pin_e pin)
{
	return (1 << (pin & IO_PIN_MASK));
}

void IO_InitMcu(void)
{
	io_config errLed = {
		.dir    = IO_DIR_OUTPUT,
		.output = IO_OUTPUT_HIGH
	};

	IO_ConfigurePin(IO_ERR_LED, &errLed);
}

void IO_ConfigurePin(io_pin_e pin, const io_config* config)
{
	IO_SetDirection(pin, config->dir);
	IO_SetOutput(pin, config->output);
}

void IO_SetDirection(io_pin_e pin, io_dir_e direction)
{
	uint8_t port   = IO_GetPort(pin);
	uint8_t pinBit = IO_GetPinBit(pin);

	switch (direction)
	{
	case IO_DIR_OUTPUT:
		*portDirRegisters[port] |= pinBit;
		break;

	case IO_DIR_INPUT:
		*portDirRegisters[port] &= ~pinBit;
		break;
	}
}

void IO_SetOutput(io_pin_e pin, io_output_e output)
{
	uint8_t port   = IO_GetPort(pin);
	uint8_t pinBit = IO_GetPinBit(pin);

	switch (output)
	{
	case IO_OUTPUT_HIGH:
		*portOutRegisters[port] |= pinBit;
		break;
	case IO_OUTPUT_LOW:
		*portOutRegisters[port] &= ~pinBit;
		break;
	}
}

#define BAUDERATE_9600 129u

void IO_InitUSART(void)
{
	//Set the baudrate
	//Set the baudrate to 9800 bps using external 20MHz RC Oscillator
	//UBRR0H and UBRR0L registers are a 16 bit register pair
	///in case the baudrate register value is greater then 255, that means that the value is greater then 8 bits
	//so by bit shifting 8 to the right we remove the first byte and focus on the second byte in the 16 bit value
	UBRR0H = (BAUDERATE_9600 >> 8);
	UBRR0L = BAUDERATE_9600; //if the value is greater then 255 then thoose bits will be discarded when setting the low bit register

	//Enable receiver and transmitter
	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);

	//USART Mode Selection : Asynchronous
	UCSR0C &= ~(1 << UMSEL01);
	UCSR0C &= ~(1 << UMSEL00);
}

uint8_t IO_USART_Receive(void)
{
	return 0;
}

void IO_USART_Transmit(uint8_t data)
{
}
