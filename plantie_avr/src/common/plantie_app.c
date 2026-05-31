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

//TODO: load in the correct ip address from eeprom
static char ipAddress[15] = "255.255.255.255";
static uint8_t ipSize     = sizeof(ipAddress); //does not have null termanation

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

	if (strstr((char*)msg.data, "SET_IPV4=\"") != NULL)
	{
		memset(ipAddress, 0, sizeof(ipAddress));
		ipSize = 0;
		//length of SET_IPV4=\" is 10 if you diregard the null termanation
		char* p        = (char*)&msg.data[10];
		uint8_t ipIndx = 0;
		while (*p != '\"' && *p != '\r')
		{
			ipAddress[ipIndx] = *p;
			ipIndx++;
			p++;
		}

		ipSize = ipIndx;

		uart_QueueTxStrIE(IO_UART_PC_TX, "New IP address set: ");
		uart_QueueTxBinIE(IO_UART_PC_TX, (uint8_t*)ipAddress, ipSize);
		uart_QueueTxStrIE(IO_UART_PC_TX, "\r\n");

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
	g_mqtt_ongoing       = true;
	char subMsgStart[19] = "AT+CIPSTART=\"TCP\",\"";
	//ip addres between
	char subMsgEnd[10] = "\",1883,1\r\n";

	char connectCmd[44] = { 0 };
	strncpy(connectCmd, subMsgStart, sizeof(subMsgStart));
	strncpy(connectCmd + sizeof(subMsgStart), ipAddress, ipSize);
	strncpy(connectCmd + (sizeof(subMsgStart) + ipSize), subMsgEnd, sizeof(subMsgEnd));
	connectCmd[43] = '\0';

	uart_QueueTxStrIE(IO_UART_ESP_TX, connectCmd);
	//TODO: store ip address in EEPROM
	// uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPSTART=\"TCP\",\"81.237.219.29\",1883,1\r\n");
}
