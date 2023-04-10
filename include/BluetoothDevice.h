/***
 * @author @MarieClaude1234
 * @file BluetoothDevice.h
 * @date 28 mars 2023
***/

#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

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

#include "Messages.h"

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
// #define SPP_SHOW_MODE SPP_SHOW_SPE

// class BluetoothDevice: public Com
// {
    // public:
        // BluetoothDevice(QueueHandle_t _queueBT_UART, QueueHandle_t _queueUART_BT, SemaphoreHandle_t _mutexBT_UART, SemaphoreHandle_t _mutexUART_BT);
        //~BluetoothDevice();
    // private:
        //void initBT(QueueHandle_t _queueBT_UART, QueueHandle_t _queueUART_BT, SemaphoreHandle_t _mutexBT_UART, SemaphoreHandle_t _mutexUART_BT);

        QueueHandle_t queueBT_UART;
        QueueHandle_t queueUART_BT;
        SemaphoreHandle_t mutexUART_BT;
        SemaphoreHandle_t mutexBT_UART;

        void initBT();
        
        void envoyerOctet(char octet);
        void envoyerBit(bool bit);
        char recevoirOctet();
        bool recevoirBit();
        void envoyerStart();
        void envoyerStop();
        void envoyerAck();
        void envoyerMessage();
        void recevoirMessage(uint8_t* data, uint8_t length);
// };

#endif