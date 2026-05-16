#include "esp_mqtt_state_handler.h"

#include "plantie_globals.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <string.h>

typedef enum
{
	MQTT_CONNECTING_CLIENT,
	MQTT_CLIENT_CONNECTED,
	MQTT_DONE
} MQTT_STATE;

typedef struct
{
	MQTT_STATE state;
} MQTT_HANDLER;

static MQTT_HANDLER handler = {
	.state = MQTT_CONNECTING_CLIENT
};

static inline void mqtt_DoneIE(UART_MSG* msg)
{
	g_mqtt_ongoing = false;
	handler.state  = MQTT_CONNECTING_CLIENT;

	uart_EmptyBufferIE(IO_UART_ESP_TX);
	uart_EmptyBufferIE(IO_UART_ESP_RX);

	PLANTIE_FLAGS |= MQTT_FINISHED;
}

static inline void mqtt_ClientConnectedIE(UART_MSG* msg)
{
	if (strstr((char*)msg->data, "ERROR") != NULL)
	{
		handler.state = MQTT_DONE;

		return;
	}

	uint8_t mqtt_publish_pkt[18] = {
		0x30, 0x10,

		0x00, 0x0B, // topic length = 11

		'p', 'l', 'a', 'n', 't', 'i', 'e', '/', 'a', 'd', 'c',

		g_plantieAdcStrValue[0], g_plantieAdcStrValue[1], g_plantieAdcStrValue[2]
	};

	_delay_ms(30);
	uart_QueueTxBinIE(IO_UART_ESP_TX, mqtt_publish_pkt, sizeof(mqtt_publish_pkt));

	if (strstr((char*)msg->data, "SEND OK") != NULL)
	{
		handler.state = MQTT_DONE;
	}
}

static inline void mqtt_ConnectClientIE(UART_MSG* msg)
{
	if (strstr((char*)msg->data, "ERROR") != NULL)
	{
		handler.state = MQTT_DONE;

		return;
	}

	if (strstr((char*)msg->data, ">") != NULL)
	{
		uart_QueueTxStrIE(IO_UART_PC_TX, "connection ready\r\n");

		_delay_ms(30);

		uint8_t mqtt_connect_pkt[18] = {
			0x10, 0x10, // CONNECT, remaining length = 16

			0x00, 0x04,
			0x4D, 0x51, 0x54, 0x54,

			0x04,
			0x02,
			0x00, 0x3C,

			0x00, 0x04,
			0x61, 0x76, 0x72, 0x31
		};
		uart_QueueTxBinIE(IO_UART_ESP_TX, mqtt_connect_pkt, sizeof(mqtt_connect_pkt));

		return;
	}

	if (strstr((char*)msg->data, "+IPD,4") != NULL)
	{
		handler.state = MQTT_CLIENT_CONNECTED;

		_delay_ms(30);
		uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPSEND=18\r\n");

		return;
	}

	if (strncmp((char*)msg->data, "CONNECT", 7) == 0)
	{
		_delay_ms(30);
		uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPSEND=18\r\n");

		return;
	}
}

void mqtt_Process(UART_MSG* msg)
{
	if (strstr((char*)msg->data, "CLOSED") != NULL)
	{
		handler.state = MQTT_DONE;

		mqtt_DoneIE(msg);

		return;
	}

	switch (handler.state)
	{
	case MQTT_CONNECTING_CLIENT:
		mqtt_ConnectClientIE(msg);
		break;

	case MQTT_CLIENT_CONNECTED:
		mqtt_ClientConnectedIE(msg);
		break;

	case MQTT_DONE:
		mqtt_DoneIE(msg);
		break;

	default:
		break;
	}
}