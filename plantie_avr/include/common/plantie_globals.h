#ifndef PLANTIE_GLOBALS_H
#define PLANTIE_GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define ADC_DATA_RDY   0x01u
#define ESP_RX_MSG_RDY 0x2u
#define MQTT_START     0x4u
#define PC_RX_MSG_RDY  0x8u
#define MQTT_FINISHED  0x16u

extern volatile uint8_t PLANTIE_FLAGS;

extern char g_plantieAdcStrValue[16];
extern bool g_mqtt_ongoing;

#endif //PLANTIE_GLOBALS_H