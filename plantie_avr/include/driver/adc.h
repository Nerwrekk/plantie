#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_Init(void);
void adc_StartConversion(void);
void adc_WaitUntilFinished(void);
uint16_t adc_GetRawData(void);
double adc_GetVoltageData(uint16_t rawData);

#endif //ADC_H
