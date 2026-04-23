#include "plantie_io.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	IO_InitMcu();

	for (;;)
	{
		IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
		_delay_ms(100);
		IO_SetOutput(IO_ERR_LED, IO_OUTPUT_LOW);
		_delay_ms(100);
	}

	// DDRB        = (1 << PIN0);
	// uint8_t pin = 0x01;

	// for (;;)
	// {
	// 	PORTB |= pin;
	// 	_delay_ms(100);
	// 	PORTB &= ~pin;
	// 	_delay_ms(100);
	// }
}