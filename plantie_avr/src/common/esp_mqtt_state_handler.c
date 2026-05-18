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

	// _delay_ms(300);
	PLANTIE_FLAGS |= MQTT_FINISHED; //investigate, why is this flag needed when it does nothing?
}

static inline void mqtt_ClientConnectedIE(UART_MSG* msg)
{
	if (strstr((char*)msg->data, "ERROR") != NULL)
	{
		handler.state = MQTT_DONE;

		return;
	}

	uint8_t mqtt_publish_req[18] = {
		0x30, //left nibble: MQTT Control Packet type: publish, right nibble: DUP, QoS level and retain flags
		0x10, //remaining length

		0x00, 0x0B, // mqtt topic, length = 11

		'p', 'l', 'a', 'n', 't', 'i', 'e', '/', 'a', 'd', 'c',

		//payload
		g_plantieAdcStrValue[0], g_plantieAdcStrValue[1], g_plantieAdcStrValue[2]
	};

	_delay_ms(30);
	uart_QueueTxBinIE(IO_UART_ESP_TX, mqtt_publish_req, sizeof(mqtt_publish_req));

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

		uint8_t mqtt_connect_req[21] = {
			0x10, //MQTT Control Packet type: Connect
			0x13, //remaining length = 19

			0x00, 0x04, //Protocol Name
			'M', 'Q', 'T', 'T',

			0x04, //Protocol Level
			0x02, //Connect Flags, Clean Session flag set

			0x00, 0x3C, //Keep Alive bytes, currently set to 60 sec

			0x00, 0x07,                       //Payload length
			'p', 'l', 'a', 'n', 't', 'i', 'e' //Client id
		};
		uart_QueueTxBinIE(IO_UART_ESP_TX, mqtt_connect_req, sizeof(mqtt_connect_req));

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
		uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPSEND=21\r\n");

		return;
	}
}

void mqtt_Process(UART_MSG* msg)
{
	if (strstr((char*)msg->data, "CLOSED") != NULL)
	{
		handler.state = MQTT_DONE;
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