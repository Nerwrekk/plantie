#include "plantie_app.h"

#include "plantie_io.h"
#include "plantie_globals.h"
#include "plantie_util.h"
#include "usart.h"
#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

void app_HandleAdcDataRdy()
{
	uint16_t data = adc_GetRawData();
	char buf[16];
	PUtil_Uint16ToAscii(data, buf, sizeof(buf));

	uart_QueueTxStrIE(IO_UART_PC_TX, buf);
	uart_TransmitIE(IO_UART_PC_TX, '\r');

	//TODO: Handle mqtt send to esp-01
}

void app_HandlePcRxMsgRdy()
{
	uart_QueueTxStrIE(IO_UART_PC_TX, "RX recieved\r");

	UART_MSG msg = { 0 };
	uart_GetCompleteRxMsg(IO_UART_PC_RX, &msg);
	uart_QueueTxMsgIE(IO_UART_PC_TX, &msg);

	if (strncmp((char*)msg.data, "AT", 2) == 0)
	{
		uart_QueueTxMsgIE(IO_UART_ESP_TX, &msg);

		return;
	}

	if (strcmp((char*)msg.data, "HIGH\r\n") == 0)
	{
		IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);

		return;
	}

	if (strcmp((char*)msg.data, "LOW\r\n") == 0)
	{
		IO_SetOutput(IO_ERR_LED, IO_OUTPUT_LOW);

		return;
	}
}

void app_HandleEspRxMsgRdy()
{
	UART_MSG msg = { 0 };
	uart_GetCompleteRxMsg(IO_UART_ESP_RX, &msg);
	uart_QueueTxMsgIE(IO_UART_PC_TX, &msg);
}
