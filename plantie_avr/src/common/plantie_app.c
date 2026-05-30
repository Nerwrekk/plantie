#include "plantie_app.h"

#include "plantie_io.h"
#include "plantie_globals.h"
#include "plantie_util.h"
#include "usart.h"
#include "adc.h"
#include "esp_mqtt_state_handler.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>

void app_HandleAdcDataRdy()
{
	uint16_t data = adc_GetRawData();
	PUtil_Uint16ToAscii(data, g_plantieAdcStrValue, sizeof(g_plantieAdcStrValue));

	//TODO: Handle mqtt send to esp-01
	PLANTIE_FLAGS |= (MQTT_START);
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
	uart_QueueTxStrIE(IO_UART_PC_TX, "\r");

	if (g_mqtt_ongoing)
	{
		mqtt_Process(&msg);
	}
}

void app_HandleMqttConnection(void)
{
	// if (g_mqtt_ongoing)
	// {
	// 	return;
	// }

	g_mqtt_ongoing = true;
	//TODO: store ip address in EEPROM
	uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPSTART=\"TCP\",\"217.208.171.171\",1883,1\r\n");

	// uart_TransmitMsgPoll(IO_UART_PC_TX, "AT+CIPSTART=\"TCP\",\"90.230.137.237\",1883,1\r\n");
	// UART_MSG msg = { 0 };
	// uart_PollEntireMsg(IO_UART_ESP_RX, &msg);

	// do
	// {
	// 	uart_PollEntireMsg(IO_UART_ESP_RX, &msg);
	// 	uart_TransmitEntireMsg(IO_UART_PC_TX, &msg);
	// } while (strstr((char*)msg.data, "CONNECT") == NULL);

	// mqtt_Process(&msg);
}
