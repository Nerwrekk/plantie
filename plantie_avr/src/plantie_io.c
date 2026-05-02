#include "plantie_io.h"
#include "adc.h"

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

	IO_InitUSART();

	//Init ADC
	io_config adcConfig = {
		.dir    = IO_DIR_INPUT,
		.output = IO_OUTPUT_LOW
	};

	IO_ConfigurePin(IO_ADC0, &adcConfig);
	IO_ConfigurePin(IO_ADC1, &adcConfig);
	IO_ConfigurePin(IO_ADC2, &adcConfig);
	IO_ConfigurePin(IO_ADC3, &adcConfig);
	IO_ConfigurePin(IO_ADC4, &adcConfig);
	IO_ConfigurePin(IO_ADC5, &adcConfig);
	IO_ConfigurePin(IO_ADC6, &adcConfig);
	IO_ConfigurePin(IO_ADC7, &adcConfig);

	ADC_Init();
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

char IO_USART_Receive(void)
{
	//Wait for incoming data
	while ((UCSR0A & (1 << RXC0)) == 0);

	return UDR0;
}

void IO_USART_Transmit(char data)
{
	//Wait for empty transmit buffer
	while ((UCSR0A & (1 << UDRE0)) == 0);

	//Put data into buffer, sends the data
	UDR0 = data;
}

void IO_USART_TransmitMsg(char* data)
{
	int indx = 0;
	while (data[indx] != '\0')
	{
		IO_USART_Transmit(data[indx]);
		++indx;
	}
}
