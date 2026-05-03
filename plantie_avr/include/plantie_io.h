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
} io_generic_pin_e;

typedef enum
{
	IO_ERR_LED   = IO_PB_0,
	IO_UART_RXD0 = IO_PD_0,
	IO_UART_TXD0 = IO_PD_1,
	IO_ADC0      = IO_PA_0,
	IO_ADC1      = IO_PA_1,
	IO_ADC2      = IO_PA_2,
	IO_ADC3      = IO_PA_3,
	IO_ADC4      = IO_PA_4,
	IO_ADC5      = IO_PA_5,
	IO_ADC6      = IO_PA_6,
	IO_ADC7      = IO_PA_7,
} io_pin_e;

typedef enum
{
	IO_PORTA,
	IO_PORTB,
	IO_PORTC,
	IO_PORTD
} io_port_e;

typedef enum
{
	IO_DIR_INPUT,
	IO_DIR_OUTPUT
} io_dir_e;

typedef enum
{
	IO_OUTPUT_HIGH,
	IO_OUTPUT_LOW
} io_output_e;

typedef enum
{
	IO_INPUT_LOW,
	IO_INPUT_HIGH
} io_input_e;

typedef struct
{
	io_dir_e dir;
	io_output_e output;
} io_config;

void IO_InitMcu(void);
void IO_ConfigurePin(io_pin_e pin, const io_config* config);
void IO_SetDirection(io_pin_e pin, io_dir_e direction);
void IO_SetOutput(io_pin_e pin, io_output_e output);
io_input_e IO_GetInput(io_pin_e pin);

#endif //PLANTIE_IO_H