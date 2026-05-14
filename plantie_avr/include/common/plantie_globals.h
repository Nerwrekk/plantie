#ifndef PLANTIE_GLOBALS_H
#define PLANTIE_GLOBALS_H

#include <stdint.h>

#define ADC_DATA_RDY   0x01u
#define ESP_RX_MSG_RDY 0x2u
#define PC_RX_MSG_RDY  0x8u

extern volatile uint8_t PLANTIE_FLAGS;

#endif //PLANTIE_GLOBALS_H