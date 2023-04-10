/***
 * @author @MarieClaude1234
 * @file BluetoothDevice.h
 * @date 2 avril 2023
***/

#ifndef MESSAGES_H
#define MESSAGES_H

#define MODE_MANUEL 0
#define MODE_AUTOMATIQUE 1

union MessageESP_OPENCR
{
    struct {
        bool mode; //manuel ou automatique
        uint8_t commande : 6; // commande a effectuer ou nombre de sequences
        bool parite; //paire
    } data;
    char raw;
};

union MessageOPENCR_ESP
{
    struct {
        bool SOF;
        bool mode;//manuel ou automatique
        uint8_t commande; //commande effectuee ou nb de sequences faites
        uint8_t etape : 3; //etape en cours
        bool electro1;
        bool electro2;
        bool electro3;
        bool parite;
        bool END;
    }data;
    // char raw[9];

};

union MessageESP_GUI
{
    struct {
        uint8_t SOF : 2;
        uint8_t etape : 3; //etape en cours
        bool electro1;
        bool electro2;
        bool electro3;
        uint64_t addr_mac : 48;
        uint16_t distance : 16; // distance parcourue
        bool parite;
        uint8_t END : 3;
    }data;
    // char raw

};

union MessageGUI_ESP
{
    struct {
        uint8_t SOF : 2;
        uint8_t commande : 6; // commande a effectuer ou nombre de sequences
        uint64_t addr_mac : 48;
        uint16_t distance : 12; // distance parcourue
        bool parite;
        uint8_t END : 3;
        uint8_t length;
    }donnees;
    struct {
        uint8_t* frame;
        uint8_t length;
    }raw;
};

#endif
