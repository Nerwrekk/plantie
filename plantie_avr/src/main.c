#include "plantie_io.h"
#include "usart.h"
#include "plantie_globals.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

int main(void)
{
	IO_InitMcu();
	//Enable global interrupt
	sei();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	USART_TransmitMsgIE(IO_UART_TXD0, "Ready for input");

	for (;;)
	{
		if ((PLANTIE_FLAGS & RX_MSG_RDY) > 0)
		{
			USART_TransmitMsgPoll(IO_UART_TXD0, "RX recieved\r");
			cli();
			RX_MSG msg = { 0 };
			USART_GetCompleteRxMsg(&msg);
			USART_SendCompleteRxMsg(IO_UART_TXD0, &msg);
			PLANTIE_FLAGS &= ~(RX_MSG_RDY);

			if (strcmp((char*)msg.data, "AT+HIGH\r") == 0)
			{
				IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
			}

			if (strcmp((char*)msg.data, "AT+LOW\r") == 0)
			{
				IO_SetOutput(IO_ERR_LED, IO_OUTPUT_LOW);
			}
			sei();
		}
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