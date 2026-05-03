#ifndef PLANTIE_IO_H
#define PLANTIE_IO_H

#include <stdint.h>

typedef enum
{
	IO_PA_0,
	IO_PA_1,
	IO_PA_2,
	IO_PA_3,
	IO_PA_4,
	IO_PA_5,
	IO_PA_6,
	IO_PA_7,
	IO_PB_0,
	IO_PB_1,
	IO_PB_2,
	IO_PB_3,
	IO_PB_4,
	IO_PB_5,
	IO_PB_6,
	IO_PB_7,
	IO_PC_0,
	IO_PC_1,
	IO_PC_2,
	IO_PC_3,
	IO_PC_4,
	IO_PC_5,
	IO_PC_6,
	IO_PC_7,
	IO_PD_0,
	IO_PD_1,
	IO_PD_2,
	IO_PD_3,
	IO_PD_4,
	IO_PD_5,
	IO_PD_6,
	IO_PD_7
} IO_GENERIC_PIN;

typedef enum
{
	IO_ERR_LED   = IO_PB_0,
	IO_UART_RXD0 = IO_PD_0,
	IO_UART_TXD0 = IO_PD_1,
	IO_UART_RXD1 = IO_PD_2,
	IO_UART_TXD1 = IO_PD_3,
	IO_ADC0      = IO_PA_0,
	IO_ADC1      = IO_PA_1,
	IO_ADC2      = IO_PA_2,
	IO_ADC3      = IO_PA_3,
	IO_ADC4      = IO_PA_4,
	IO_ADC5      = IO_PA_5,
	IO_ADC6      = IO_PA_6,
	IO_ADC7      = IO_PA_7,
} IO_PIN;

typedef enum
{
	IO_PORTA,
	IO_PORTB,
	IO_PORTC,
	IO_PORTD
} IO_PORT;

typedef enum
{
	IO_DIR_INPUT,
	IO_DIR_OUTPUT
} IO_DIR;

typedef enum
{
	IO_OUTPUT_HIGH,
	IO_OUTPUT_LOW
} IO_OUTPUT;

typedef enum
{
	IO_INPUT_LOW,
	IO_INPUT_HIGH
} IO_INPUT;

typedef struct
{
	IO_DIR dir;
	IO_OUTPUT output;
} IO_CONFIG;

void IO_InitMcu(void);
void IO_ConfigurePin(IO_PIN pin, const IO_CONFIG* config);
void IO_SetDirection(IO_PIN pin, IO_DIR direction);
void IO_SetOutput(IO_PIN pin, IO_OUTPUT output);
IO_INPUT IO_GetInput(IO_PIN pin);

#endif //PLANTIE_IO_H