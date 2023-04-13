/***
 * @author @MarieClaude1234
 * @file main.c
 * @date 1 avril 2023
***/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../include/Messages.h"
// #include "../include/Utilities.h"
#include "BTDevice.h"
#include "UartDevice.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

SemaphoreHandle_t mutexUART_BT;
SemaphoreHandle_t mutexBT_UART;
QueueHandle_t queueBT_UART;
QueueHandle_t queueUART_BT;

TaskHandle_t handleTX;

void app_main() {
    // Initialise les mutex
    mutexUART_BT = xSemaphoreCreateMutex();
    mutexBT_UART = xSemaphoreCreateMutex();

    // Initialise les files de messages
    queueBT_UART = xQueueCreate(8, sizeof(struct MessageESP_OPENCR));
    queueUART_BT = xQueueCreate(8, sizeof(struct MessageESP_GUI));

    initUART();
    initBT();

    xTaskCreate(txUartTask, "uart_send_data_task", UART_TX_STACK_SIZE, NULL, UART_TX_PRIORITY, NULL);
    xTaskCreate(rxUartTask, "uart_receive_data_task", UART_RX_STACK_SIZE, NULL, UART_RX_PRIORITY, NULL);
    xTaskCreate(txBtTask, "bt_sende_data_task", 2048, NULL, 1, NULL);
}