/***
 * @author @trudelle200902
 * @file BluetoothDevice.cpp
 * @date 05 avril 2023
***/

#include "UartDevice.h"



void init_uart(){    
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

static void tx_task(void * arg){
    struct something_something* my_struct;
    my_struct = (struct something_something*) arg;

    uint8_t tx_data = 4;
    while (1) {
        //uart_write_bytes(UART_PORT, tx_data, strlen(tx_data));
        uart_write_bytes(UART_PORT, &tx_data, sizeof(tx_data));
        vTaskDelay(pdMS_TO_TICKS(UART_TX_DELAY));
    }
}

static void rx_task(void * arg){
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