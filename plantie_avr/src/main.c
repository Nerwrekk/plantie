#include "plantie_io.h"
#include "usart.h"
#include "adc.h"
#include "plantie_globals.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
	IO_InitMcu();
	//Enable global interrupt
	sei();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	USART_TransmitMsgIE(IO_UART_TXD0, "Ready for input");

	//ADC values
	//wet == 721
	//dry == 895

	ADC_StartConversion();

	for (;;)
	{
		if ((PLANTIE_FLAGS & ADC_DATA_RDY) > 0)
		{
			PLANTIE_FLAGS &= ~(ADC_DATA_RDY);
			cli();
			uint16_t data = ADC_GetRawData();
			char buf[8];
			itoa(data, buf, 10);
			buf[7] = '\0';
			USART_TransmitMsgPoll(IO_UART_TXD0, buf);
			USART_TransmitPoll(IO_UART_TXD0, '\r');
			sei();
		}

		if ((PLANTIE_FLAGS & PC_RX_MSG_RDY) > 0)
		{
			USART_TransmitMsgPoll(IO_UART_TXD0, "RX recieved\r");
			cli();
			RX_MSG msg = { 0 };
			USART_GetCompleteRxMsg(&msg);
			USART_SendCompleteRxMsg(IO_UART_TXD0, &msg);
			PLANTIE_FLAGS &= ~(PC_RX_MSG_RDY);

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