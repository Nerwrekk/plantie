#include "plantie_io.h"
#include "plantie_globals.h"
#include "plantie_app.h"
#include "usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

static inline void Plantie_ClearFlag(uint8_t flag)
{
	uint8_t sreg = SREG;
	cli();
	PLANTIE_FLAGS &= ~(flag);
	SREG = sreg; //restore glob interrupts
}

static inline void ConfigEsp01(void)
{
	//ESP-01 configuration

	//Enable/disable Multiple Connections,  0: single connection, 1: multiple connections
	//TODO: change this if we want to support subscribing to mqtt events
	_delay_ms(30);
	uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPMUX=0\r\n");

	//Echoing, ATE0: Switch echo off, ATE1: Switch echo on.
	_delay_ms(30);
	uart_QueueTxStrIE(IO_UART_ESP_TX, "ATE0\r\n");

	//Set Socket Receiving Mode, set to 1: passive mode
	_delay_ms(30);
	uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPRECVMODE=1\r\n");
}

int main(void)
{
	IO_InitMcu();
	//Enable global interrupt
	sei();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	uart_QueueTxStrIE(IO_UART_PC_TX, "Ready for input\r\n");

	ConfigEsp01();

	//ADC values
	//wet == 721
	//dry == 895

	for (;;)
	{
		if ((PLANTIE_FLAGS & ADC_DATA_RDY))
		{
			Plantie_ClearFlag(ADC_DATA_RDY);

			app_HandleAdcDataRdy();
		}

		if ((PLANTIE_FLAGS & PC_RX_MSG_RDY))
		{
			Plantie_ClearFlag(PC_RX_MSG_RDY);

			app_HandlePcRxMsgRdy();
		}

		if ((PLANTIE_FLAGS & ESP_RX_MSG_RDY))
		{
			Plantie_ClearFlag(ESP_RX_MSG_RDY);

			app_HandleEspRxMsgRdy();
		}

		if ((PLANTIE_FLAGS & MQTT_START))
		{
			Plantie_ClearFlag(MQTT_START);

			app_HandleMqttConnection();
		}
	}
}