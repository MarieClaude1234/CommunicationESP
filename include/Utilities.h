/***
 * @author @MarieClaude1234
 * @file Utilities.h
 * @date 12 avril 2023
***/

#ifndef UTILITIES_H
#define UTILITIES_H

#include "freertos/queue.h"
#include "freertos/semphr.h"

SemaphoreHandle_t mutexUART_BT;
SemaphoreHandle_t mutexBT_UART;
QueueHandle_t queueBT_UART;
QueueHandle_t queueUART_BT;

#endif