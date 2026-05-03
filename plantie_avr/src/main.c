#include "plantie_io.h"
#include "usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	IO_InitMcu();
	//Enable global interrupt
	sei();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);

	for (;;)
	{
		// USART_TransmitIE('i');
		USART_TransmitMsgIE(IO_UART_TXD0, "interrupt hello\r");
		_delay_ms(1000);
	}
	// for (;;)
	// {
	// 	USART_TransmitMsgPoll("Ready to speak");

	// 	char data = USART_ReceivePoll();
	// 	if (data == 'h')
	// 	{
	// 		IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	// 	}

	// 	if (data == 'l')
	// 	{
	// 		IO_SetOutput(IO_ERR_LED, IO_OUTPUT_LOW);
	// 	}
	// }
}