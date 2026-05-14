#include "plantie_io.h"
#include "usart.h"
#include "adc.h"
#include "plantie_globals.h"
#include "plantie_util.h"

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
	uart_QueueTxStrIE(IO_UART_PC_TX, "Ready for input");

	//ADC values
	//wet == 721
	//dry == 895

	// ADC_StartConversion();

	for (;;)
	{
		if ((PLANTIE_FLAGS & ADC_DATA_RDY))
		{
			cli();
			PLANTIE_FLAGS &= ~(ADC_DATA_RDY);
			uint16_t data = ADC_GetRawData();
			char buf[8];
			PUtil_Uint16ToAscii(data, buf, sizeof(buf));
			uart_TransmitMsgPoll(IO_UART_PC_TX, buf);
			uart_TransmitPoll(IO_UART_PC_TX, '\r');
			sei();
		}

		if ((PLANTIE_FLAGS & PC_RX_MSG_RDY))
		{
			cli();
			PLANTIE_FLAGS &= ~(PC_RX_MSG_RDY);
			sei();
			uart_TransmitMsgPoll(IO_UART_PC_TX, "RX recieved\r");
			UART_MSG msg = { 0 };
			uart_GetCompleteRxMsg(IO_UART_PC_RX, &msg);
			uart_SendCompleteMsgPoll(IO_UART_PC_TX, &msg);

			if (strcmp((char*)msg.data, "HIGH\r\n") == 0)
			{
				IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
			}

			if (strcmp((char*)msg.data, "LOW\r\n") == 0)
			{
				IO_SetOutput(IO_ERR_LED, IO_OUTPUT_LOW);
			}

			if (strncmp((char*)msg.data, "AT", 2) == 0)
			{
				uart_SendCompleteMsgPoll(IO_UART_ESP_TX, &msg);
				// USART_TransmitMsgIE(IO_UART_TXD1, (char*)msg.data);
			}
		}

		if ((PLANTIE_FLAGS & ESP_RX_MSG_RDY))
		{
			cli();
			PLANTIE_FLAGS &= ~(ESP_RX_MSG_RDY);
			UART_MSG msg = { 0 };
			uart_GetCompleteRxMsg(IO_UART_ESP_RX, &msg);
			uart_SendCompleteMsgPoll(IO_UART_PC_TX, &msg);

			sei();
		}
	}
}