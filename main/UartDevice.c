/***
 * @author @trudelle200902
 * @file BluetoothDevice.cpp
 * @date 05 avril 2023
***/

#include "UartDevice.h"

#define UART_TAG "UART_TAG"

void initUART(){    
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RX_BUFF_SIZE, UART_TX_BUFF_SIZE, 0, NULL, 0));
}

void txTask(void * arg){
    // struct something_something* my_struct;
    // my_struct = (struct something_something*) arg;


    while (1) {
        struct MessageESP_OPENCR transfert = {0,0,0};
        bool nouvMessage = false;
        
        xSemaphoreTake(mutexBT_UART, (TickType_t) 25);
        if(uxQueueMessagesWaiting(queueBT_UART) > 0){
            xQueueReceive(queueBT_UART, &transfert, (TickType_t) 25);
            nouvMessage = true;
        }
        xSemaphoreGive(mutexBT_UART);

        if(nouvMessage){
            uint8_t tx_data = (transfert.mode << 7) + (transfert.commande << 1) + transfert.parite;
            ESP_LOGI(UART_TAG, "mode : %d, commande : %d, parite : %d, tx_data : %d", transfert.mode, transfert.commande, transfert.parite);
            uart_write_bytes(UART_PORT, &tx_data, sizeof(tx_data));
        }

        vTaskDelay(pdMS_TO_TICKS(UART_TX_DELAY));
    }
}

void rxTask(void * arg){
    struct concurrency_handler* handle;
    handle = (struct concurrency_handler*) arg;
    
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);

    uint8_t data[UART_RX_BUFF_SIZE];
    while (1) {
        int rx_bytes = uart_read_bytes(UART_PORT, data, UART_RX_BUFF_SIZE, pdMS_TO_TICKS(UART_RX_DELAY));
        if (rx_bytes > 0) {
            //printf("Received %d bytes: ", rx_bytes);
            for (int i = 0; i < rx_bytes; i++)
            {
                //printf("%02X ", data[i]);
            }
            //printf("\n");
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: %u", rx_bytes, data[0]);
        }
    }
}