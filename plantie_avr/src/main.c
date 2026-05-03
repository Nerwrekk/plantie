#include "plantie_io.h"
#include "usart.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	IO_InitMcu();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	for (;;)
	{
		USART_TransmitMsgPoll("Ready to speak");

		char data = USART_ReceivePoll();
		if (data == 'h')
		{
			IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
		}

		if (data == 'l')
		{
			IO_SetOutput(IO_ERR_LED, IO_OUTPUT_LOW);
		}
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