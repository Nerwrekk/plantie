#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void ADC_Init(void);
void ADC_StartConversion(void);
void ADC_WaitUntilFinished(void);
uint16_t ADC_GetRawData(void);
double ADC_GetVoltageData(uint16_t rawData);

#endif //ADC_H
