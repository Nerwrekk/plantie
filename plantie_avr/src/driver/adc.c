#include "adc.h"
#include "plantie_globals.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION_10_BIT_VOLTAGE 1024.0

ISR(ADC_vect)
{
	PLANTIE_FLAGS |= ADC_DATA_RDY;
	ADC_StartConversion();
}

void ADC_Init(void)
{
	//Voltage Reference Selection set to AVCC with external capacitor at AREF pin
	//ADLAR set to right Adjust Result
	ADMUX = 0x40;

	//ADC Enable,  Interrupt Enable, ADC Prescaler Select bit all set to 1
	ADCSRA = 0x8F;
}

void ADC_StartConversion(void)
{
	//Set ADSC bit to 1 to begin the conversion
	ADCSRA |= (1 << ADSC);
}

void ADC_WaitUntilFinished(void)
{
	//wait until ADSC bit is set to 0 which indicates that the conversion is finished
	while ((ADCSRA & (1 << ADSC)) != 0);
}

uint16_t ADC_GetRawData(void)
{
	//ADLAR set to right Adjust Result
	return (ADCL) | (ADCH << 8);
}

double ADC_GetVoltageData(uint16_t rawData)
{
	//1024.0 comes from that the atmega644p has adc 10bit resolution (0 - 1023) 0x3FF
	//we divide by 1024 as the resolution has 1024 steps
	return (rawData * 5) / RESOLUTION_10_BIT_VOLTAGE;
}
