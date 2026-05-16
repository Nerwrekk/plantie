#include "plantie_globals.h"

volatile uint8_t PLANTIE_FLAGS = 0;
char g_plantieAdcStrValue[16]  = { 0 };
bool g_mqtt_ongoing            = false;