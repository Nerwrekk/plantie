#include "plantie_io.h"
#include "adc.h"
#include "usart.h"

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

	USART_Init();

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
