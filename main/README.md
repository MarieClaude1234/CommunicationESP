# main
## main.c
Ce fichier crée les instances de mutex et de files pour toute la communication. De plus, le main appelle les fonctions **initBT()** et **initUART()**. Le main crée aussi les tâches de réception et d'envoi UART, ainsi que la tâche d'envoi message Bluetooth.

## BTDevice.h
Ce fichier contient les déclarations de fonctions pour les fonctions du module Bluetooth.

## BTDevice.c
Ce fichier contient les définitions de fonctions pour les fonctions du module Bluetooth.
Le code du module Bluetooth est tiré de l'exemple SPP_ACCEPTOR de l'ESP-IDF trouvé ici : https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/bluedroid/classic_bt/bt_spp_acceptor. 

## UARTDevice.h
Ce fichier contient les déclarations de fonctions pour les fonctions du module UART.

## UARTDevice.c
Ce fichier contient les définitions de fonctions pour les fonctions du module UART.
