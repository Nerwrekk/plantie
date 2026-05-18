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

int main(void)
{
	IO_InitMcu();
	//Enable global interrupt
	sei();

	IO_SetOutput(IO_ERR_LED, IO_OUTPUT_HIGH);
	uart_QueueTxStrIE(IO_UART_PC_TX, "Ready for input\r\n");

	//ADC values
	//wet == 721
	//dry == 895

	// ADC_StartConversion();
	uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPMUX=0\r\n");
	_delay_ms(300);
	uart_QueueTxStrIE(IO_UART_ESP_TX, "ATE1\r\n");
	_delay_ms(300);
	uart_QueueTxStrIE(IO_UART_ESP_TX, "AT+CIPRECVMODE=1\r\n");
	// _delay_ms(300);

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

		if ((PLANTIE_FLAGS & MQTT_STARTED))
		{
			uint8_t sreg = SREG;
			cli();
			PLANTIE_FLAGS &= ~(MQTT_STARTED);
			// Plantie_ClearFlag(MQTT_STARTED);

			uart_TransmitMsgPoll(IO_UART_ESP_TX, "AT+CIPCLOSE\r\n");

			uart_EmptyBufferPoll(IO_UART_ESP_RX);

			SREG = sreg;

			_delay_ms(300);

			app_HandleMqttConnection();
		}
	}
}