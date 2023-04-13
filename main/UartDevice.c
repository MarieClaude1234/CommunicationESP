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

bool verifParite2(uint8_t* data, uint8_t length){
    int parite = 0;
    for(int i = 0; i < length; i++){
        parite += data[i] & 0b00000001;
        parite += (data[i] & 0b00000010) >> 1;
        parite += (data[i] & 0b00000100) >> 2;
        parite += (data[i] & 0b00001000) >> 3;
        parite += (data[i] & 0b00010000) >> 4;
        parite += (data[i] & 0b00100000) >> 5;
        parite += (data[i] & 0b01000000) >> 6;
        parite += (data[i] & 0b10000000) >> 7;
    }

    return parite % 2;
}

bool calculParite2(uint8_t data[], uint8_t length){
    int total = 0;
    for(int i = 0; i < length; i++){
        total += data[i] & 0b00000001;
        total += (data[i] & 0b00000010) >> 1;
        total += (data[i] & 0b00000100) >> 2;
        total += (data[i] & 0b00001000) >> 3;
        total += (data[i] & 0b00010000) >> 4;
        total += (data[i] & 0b00100000) >> 5;
        total += (data[i] & 0b01000000) >> 6;
        total += (data[i] & 0b10000000) >> 7;
    }
    if(total % 2 == 0)
        return 0;
    else
        return 1;
}

uint16_t calculDistance(uint8_t sequences){
    uint16_t distance = sequences * CONVERSION_DIST_SEQ;

    return distance;
}


void txUartTask(void * arg){
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

void rxUartTask(void * arg){
    
    while (1) {
        uint16_t data;

        int rx_bytes = uart_read_bytes(UART_PORT, &data, sizeof(uint16_t), pdMS_TO_TICKS(UART_RX_DELAY));
        if (rx_bytes > 0) {
            uint8_t data2[2] = {(data & 0xFF00)>>8, data & 0xFF};
            if(verifParite2(data2, 2) == 1)
                ESP_LOGE(UART_RX_TAG, "Reception error -- parity");
            else{
                struct MessageOPENCR_ESP recu;
                recu.SOF = (data & 0b1000000000000000) >> 15;
                recu.mode = (data & 0b0100000000000000) >> 14;
                recu.commande = (data & 0b0011111100000000) >> 8;
                recu.etape = (data & 0b0000000011100000) >> 5;
                recu.electro1 = (data & 0b0000000000010000) >> 4;
                recu.electro2 = (data & 0b0000000000001000) >> 3;
                recu.electro3 = (data & 0b0000000000000100) >> 2;
                recu.parite = (data & 0b0000000000000010) >> 1;
                recu.END = data & 0b0000000000000001;
                
                ESP_LOGI(UART_RX_TAG, "SOF : %d, Mode : %d, Commande : %d, Etape : %d, E1 : %d, E2 : %d, E3 : %d, parite : %d, END : %d", 
                    recu.SOF, recu.mode, recu.commande, recu.etape, recu.electro1, recu.electro2, recu.electro3, recu.parite, recu.END);
            
                struct MessageESP_GUI transfert;
                transfert.SOF = 3;
                transfert.etape = recu.etape;
                transfert.electro1 = recu.electro1;
                transfert.electro2 = recu.electro2;
                transfert.electro3 = recu.electro3;
                // adresse mac
                long long int masque = 0xFF0000000000;
                for(int i = 0;  i < 6; i++){
                    transfert.addr_mac[i] = (ADRESSE_MAC & masque) >> (8*(6 - i - 1));
                    masque = masque >> 8;
                }
                // distance
                if(recu.mode == MODE_AUTOMATIQUE) 
                    transfert.distance = calculDistance(recu.commande);
                else
                    transfert.distance = 0;
                // end
                transfert.parite = 0;
                transfert.END = 5;
                // parite
                uint8_t copy[9];
                copy[0] = (transfert.SOF << 6) + (transfert.etape << 3) + transfert.electro1 + transfert.electro2 + transfert.electro3;
                copy[1] = transfert.addr_mac[0];
                copy[2] = transfert.addr_mac[1];
                copy[3] = transfert.addr_mac[2];
                copy[4] = transfert.addr_mac[3];
                copy[5] = transfert.addr_mac[4];
                copy[6] = transfert.addr_mac[5];
                copy[7] = (transfert.distance & 0x0FF0) >> 4;
                copy[8] = (transfert.distance & 0xF) + (transfert.parite << 3) + transfert.END;
                transfert.parite = calculParite2(&copy, LENGTH_ESP_GUI);
                            
                xSemaphoreTake(mutexUART_BT, (TickType_t) 25);
                xQueueSend(queueUART_BT, &transfert, (TickType_t) 25);
                xSemaphoreGive(mutexUART_BT);

                vTaskDelay(pdMS_TO_TICKS(UART_TX_DELAY));
            }
        }
    }
}