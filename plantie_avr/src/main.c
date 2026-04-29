#include "plantie_io.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	IO_InitMcu();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	for (;;)
	{
		IO_USART_Transmit('x');
		// IO_USART_Transmit('e');
		// IO_USART_Transmit('l');
		// IO_USART_Transmit('o');
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