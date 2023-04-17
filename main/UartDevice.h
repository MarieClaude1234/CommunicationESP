/***
 * @author @trudelle200902
 * @file UartDevice.h
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

/**
 * @brief Initialise les configurations UART
 * 
 */
void initUART();

/**
 * @brief Verifie la parite du message passe en parametres
 * 
 * @param data message recu
 * @param length nombre d'octets du message recu
 * @return retourne 1 lors d'une erreur de parite
 */
bool verifPariteUART(uint8_t* data, uint8_t length);
/**
 * @brief Calcule la parite du message passe en parametres
 * 
 * @param data message a calculer
 * @param length nombre d'octets du message recu
 * @return la valeur a donner au bit de parite
 */
bool calculPariteUART(uint8_t data[], uint8_t length);
/**
 * @brief Effectue la conversion du nombre de sequences a la distance
 * 
 * @param sequences nombre de sequences
 * @return distance en mm
 */
uint16_t calculDistance(uint8_t sequences);

/**
 * @brief Tache d'envoi de messages UART
 * 
 * @param arg structure des valeurs necessaires externes a la fonction
 */
void txUartTask(void * arg);
/**
 * @brief Tache de reception de messages UART
 * 
 * @param arg structure des valeurs necessaires externes a la fonction
 */
void rxUartTask(void * arg);

#endif