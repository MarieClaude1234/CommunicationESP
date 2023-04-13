/***
 * @author @MarieClaude1234
 * @file Utilities.h
 * @date 28 mars 2023
***/

#ifndef BTDEVICE
#define BTDEVICE

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "time.h"
#include "sys/time.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "../include/Messages.h"
// #include "../include/Utilities.h"

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define RECEPTION_TAG "SPP_RECEPTION"

extern SemaphoreHandle_t mutexUART_BT;
extern SemaphoreHandle_t mutexBT_UART;
extern QueueHandle_t queueBT_UART;
extern QueueHandle_t queueUART_BT;

void initBT();
bool verifParite(uint8_t* data, uint8_t length);
bool calculParite(uint8_t* data, uint8_t length);
uint8_t calculSequences(uint16_t distance);
bool verifMAC(uint8_t adresse[6]);
#endif