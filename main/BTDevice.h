/***
 * @author @MarieClaude1234
 * @file BtDevice.h
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

#define SPP_TAG "SPP_ACCEPTOR"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define RECEPTION_TAG "SPP_RECEPTION"
#define ENVOI_TAG "SPP_ENVOI"

extern SemaphoreHandle_t mutexUART_BT;
extern SemaphoreHandle_t mutexBT_UART;
extern QueueHandle_t queueBT_UART;
extern QueueHandle_t queueUART_BT;

/**
 * @brief Conofigure et initialise le module Bluetooth
 * 
 */
void initBT();
/**
 * @brief Verifie la parite du message passe en parametres
 * 
 * @param data message recu
 * @param length nombre d'octets du message recu
 * @return retourne 1 lors d'une erreur de parite
 */
bool verifPariteBT(uint8_t* data, uint8_t length);
/**
 * @brief Calcule la parite du message passe en parametres
 * 
 * @param data message a calculer
 * @param length nombre d'octets du message recu
 * @return la valeur a donner au bit de parite
 */
bool calculPariteBT(uint8_t* data, uint8_t length);
/**
 * @brief Effectue la conversion de la distance en nombre de sequences
 * 
 * @param distance distance en mm
 * @return nombre de sequences
 */
uint8_t calculSequences(uint16_t distance);
/**
 * @brief Verifie que l'adresse MAC recu correspond son adresse MAC
 * 
 * @param adresse adresse MAC a verifier
 * @return retourne FALSE si l'adresse MAC est incorrecte
 */
bool verifMAC(uint8_t adresse[6]);
/**
 * @brief Tache d'envoi de messages BT
 * 
 * @param arg structure des valeurs necessaires externes a la fonction
 */
void txBtTask(void * arg);
#endif