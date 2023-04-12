/***
 * @author @MarieClaude1234
 * @file main.c
 * @date 22 janvier 2023
***/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
// #include "../include/BluetoothDevice.h"
#include "../include/Messages.h"
#include "BTDevice.h"

SemaphoreHandle_t mutexUART_BT;
SemaphoreHandle_t mutexBT_UART;
QueueHandle_t queueBT_UART;
QueueHandle_t queueUART_BT;

// Initialise les peripheriques de communication
// BluetoothDevice* bt = new BluetoothDevice(queueBT_UART, queueUART_BT, mutexBT_UART, mutexUART_BT);
//UARTDevice

void app_main() {
    //xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char *const pcName, const configSTACK_DEPTH_TYPE usStackDepth, void *const pvParameters, UBaseType_t uxPriority, TaskHandle_t *const pvCreatedTask, const BaseType_t xCoreID);
    // Initialise les mutex
    mutexUART_BT = xSemaphoreCreateMutex();
    mutexBT_UART = xSemaphoreCreateMutex();

    // Initialise les files de messages
    queueBT_UART = xQueueCreate(8, sizeof(struct MessageESP_OPENCR));
    queueUART_BT = xQueueCreate(8, sizeof(struct MessageESP_GUI));

    //initBT(queueBT_UART, queueUART_BT, mutexBT_UART, mutexUART_BT);
    initBT();
}