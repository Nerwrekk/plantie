#include "timer.h"

#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define TIME_FEQ_1_SEC 19530u

static uint16_t seconds = 0;
static uint8_t minutes  = 0;

ISR(TIMER1_COMPA_vect)
{
	seconds++;

	if (seconds == 20)
	{
		seconds = 0;
		minutes++;
		if (minutes == 1)
		{
			minutes = 0;
			adc_StartConversion();
		}
	}
}

void timer_Init(void)
{
	TCCR1A = 0;
	TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12); //prescaler set to 1024, enable CTC

	TIMSK1 = (1 << OCIE1A); //enable Output Compare A Match Interrupt

	OCR1AH = (TIME_FEQ_1_SEC >> 8);   //take the high byte and set it in the high register
	OCR1AL = (TIME_FEQ_1_SEC & 0xff); //take the low byte  and set it in the low register

	//when using Output Compare A Match Interrupt its important to also enable CTC so the timer will reset after it
	//reaches the compare value.
	//Clear Timer on Compare Match (CTC)
}