/***
 * @author @MarieClaude1234
 * @file BluetoothDevice.h
 * @date 28 mars 2023
***/

#ifndef UARTDEVICE_H
#define UARTDEVICE_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "time.h"
#include "sys/time.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "../include/Messages.h"


#define UART_BAUD_RATE 9600
#define UART_TX_BUFF_SIZE 256
#define UART_RX_BUFF_SIZE 256
#define UART_TX_STACK_SIZE 2048
#define UART_RX_STACK_SIZE 2048
#define UART_RX_DELAY 500
#define UART_TX_DELAY 1000
#define UART_TX_PRIORITY 1
#define UART_RX_PRIORITY 1
#define UART_TX_PIN (17)
#define UART_RX_PIN (16)
#define UART_PORT UART_NUM_2

#define UART_RX_TAG "UART RX"

extern SemaphoreHandle_t mutexUART_BT;
extern SemaphoreHandle_t mutexBT_UART;
extern QueueHandle_t queueBT_UART;
extern QueueHandle_t queueUART_BT;

// struct concurrency_handle{
//     QueueHandle_t queueBT_UART;
//     QueueHandle_t queueUART_BT;
//     SemaphoreHandle_t mutexUART_BT;
//     SemaphoreHandle_t mutexBT_UART;
// };

void initUART();

bool verifParite2(uint8_t* data, uint8_t length);
bool calculParite2(uint8_t data[], uint8_t length);
uint16_t calculDistance(uint8_t sequences);

void txUartTask(void * arg);
void rxUartTask(void * arg);

#endif