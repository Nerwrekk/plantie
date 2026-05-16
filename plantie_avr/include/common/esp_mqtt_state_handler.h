#ifndef ESP_MQTT_STATE_HANDLER_H
#define ESP_MQTT_STATE_HANDLER_H

#include "usart.h"

void mqtt_Process(UART_MSG* msg);

//Polling

//interrupt
// void mqtt_DoneIE(UART_MSG* msg);
// void mqtt_ClientConnectedIE(UART_MSG* msg);
// void mqtt_ConnectClientIE(UART_MSG* msg);

#endif //ESP_MQTT_STATE_HANDLER_H