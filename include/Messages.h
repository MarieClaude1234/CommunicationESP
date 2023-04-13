/***
 * @author @MarieClaude1234
 * @file BluetoothDevice.h
 * @date 2 avril 2023
***/

#ifndef MESSAGES_H
#define MESSAGES_H

#define MODE_MANUEL 0
#define MODE_AUTOMATIQUE 1
#define LENGTH_ESP_OPENCR 1
#define LENGTH_OPENCR_ESP 2
#define LENGTH_ESP_GUI 9
#define LENGTH_GUI_ESP 9
#define CONVERSION_DIST_SEQ 80 // en mm
#define ADRESSE_MAC 0x807d3ab933e2

struct MessageESP_OPENCR{
    bool mode; //manuel ou automatique
    uint8_t commande : 6; // commande a effectuer ou nombre de sequences
    bool parite; //paire
};


struct MessageOPENCR_ESP{
    bool SOF;
    bool mode;//manuel ou automatique
    uint8_t commande; //commande effectuee ou nb de sequences faites
    uint8_t etape : 3; //etape en cours
    bool electro1;
    bool electro2;
    bool electro3;
    bool parite;
    bool END;
};


struct MessageESP_GUI{
    uint8_t SOF : 2;
    uint8_t etape : 3; //etape en cours
    bool electro1;
    bool electro2;
    bool electro3;
    uint64_t addr_mac : 48;
    uint16_t distance : 16; // distance parcourue
    bool parite;
    uint8_t END : 3;
};


struct MessageGUI_ESP{
    uint8_t SOF : 2;
    uint8_t commande : 6; // commande a effectuer ou nombre de sequences
    uint8_t addr_mac[6];
    uint16_t distance : 12; // distance parcourue
    bool parite;
    uint8_t END : 3;
    uint8_t length;
};

#endif
