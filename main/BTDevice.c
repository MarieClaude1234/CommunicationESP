/***
 * @author @MarieClaude1234
 * @file BtDevice.h
 * @date 28 mars 2023
***/

#include "BTDevice.h"

uint32_t connectionHandle;

void txBtTask(void * arg){
    while(1){
        struct MessageESP_GUI transfert;
        bool nouvMessage = false;
        
        xSemaphoreTake(mutexUART_BT, (TickType_t) 25);
        if(uxQueueMessagesWaiting(queueUART_BT) > 0){
            xQueueReceive(queueUART_BT, &transfert, (TickType_t) 25);
            nouvMessage = true;
        }
        xSemaphoreGive(mutexUART_BT);

        if(nouvMessage){
            uint8_t data[9];
            data[0] = (transfert.SOF << 6) + (transfert.etape << 3) + transfert.electro1 + transfert.electro2 + transfert.electro3;
            data[1] = transfert.addr_mac[0];
            data[2] = transfert.addr_mac[1];
            data[3] = transfert.addr_mac[2];
            data[4] = transfert.addr_mac[3];
            data[5] = transfert.addr_mac[4];
            data[6] = transfert.addr_mac[5];
            data[7] = (transfert.distance & 0x0FF0) >> 4;
            data[8] = (transfert.distance & 0xF) + (transfert.parite << 3) + transfert.END;
            
            if(esp_spp_write(connectionHandle, 9, data) == ESP_SPP_SUCCESS)
                ESP_LOGI(ENVOI_TAG, "Message sent -- SOF : %d, etape : %d, e1 : %d, e2 : %d, e3 : %d, distance : %d", transfert.SOF, transfert.etape, transfert.electro1, transfert.electro2, transfert.electro3, transfert.distance);
            else
                ESP_LOGE(ENVOI_TAG, "Sending error");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

bool verifPariteBT(uint8_t* data, uint8_t length){
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

bool calculPariteBT(uint8_t data[], uint8_t length){
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

uint8_t calculSequences(uint16_t distance){
    uint8_t sequences = distance / CONVERSION_DIST_SEQ;

    return sequences;
}

bool verifMAC(uint8_t adresse[6]){
    long long int masque = 0xFF0000000000;

    for(int i = 0;  i < 6; i++){
        if(adresse[i] != (ADRESSE_MAC & masque) >> (8*(6 - i - 1))){
            ESP_LOGE(RECEPTION_TAG, "octet recu : %X, octet attendu : %X, masque : %llX", adresse[i], (ADRESSE_MAC & masque) >> (8*(6 - i - 1)), masque);
            return false;
        }
        masque = masque >> 8;
    }
    return true;

}

void recevoirMessage(uint8_t data[], uint8_t length){
    if(verifParite(data, length) == 1){ // verifie que la parite est bonne
        ESP_LOGE(RECEPTION_TAG, "Reception error -- parity");
    }
    else{
        struct MessageGUI_ESP recu;
        recu.SOF = (data[0] & 0b11000000) >> 6;
        recu.commande = data[0] & 0b00111111;
        memcpy(recu.addr_mac, &data[1], 6*sizeof(uint8_t));
        recu.distance = (data[7] << 4) + ((data[8] & 0b11110000)>>4);
        recu.parite = ((data[8] & 0b00001000) >> 3);
        recu.END = (data[8] & 0b0111);
        ESP_LOGI(RECEPTION_TAG, "SOF : %d, commande : %d, MAC : %X:%X:%X:%X:%X:%X, distance : %d, parite : %d, END: %d", recu.SOF, recu.commande, recu.addr_mac[0], recu.addr_mac[1], recu.addr_mac[2], recu.addr_mac[3], recu.addr_mac[4], recu.addr_mac[5], recu.distance, recu.parite, recu.END);

        if(verifMAC(recu.addr_mac) == true){ // verifie que le message s'adresse bien a lui
            struct MessageESP_OPENCR transfert;
            
            if((recu.commande) >= 15){ // mode auto
                transfert.mode = MODE_AUTOMATIQUE;
                if(recu.commande == 16){
                    transfert.commande = calculSequences(recu.distance);
                }
                else if(recu.commande == 17){
                    transfert.commande = 0;
                }
                else if (recu.commande == 15)
                {
                    transfert.commande = 0b111111111111;
                }  
            }
            else{ // mode manuel
                transfert.mode = MODE_MANUEL;
                transfert.commande = recu.commande;
            }
            transfert.parite = 0;

            uint8_t copy[1];
            copy[0] = (transfert.mode << 7) + (transfert.commande << 1) + transfert.parite;
            transfert.parite = calculParite(&copy, LENGTH_ESP_OPENCR);

            // ESP_LOGI(RECEPTION_TAG, "mode : %d, commande : %d, parite : %d",transfert.mode, transfert.commande, transfert.parite);

            xSemaphoreTake(mutexBT_UART, portMAX_DELAY);
            xQueueSend(queueBT_UART, &transfert, portMAX_DELAY);
            xSemaphoreGive(mutexBT_UART);

        }
    }
}

static struct timeval time_new, time_old;
static long data_num = 0;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

static char*bda2str(uint8_t * bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    char bda_str[18] = {0};
    uint8_t raw[9];

    switch (event) {
    case ESP_SPP_INIT_EVT:
        if (param->init.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
            esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_INIT_EVT status:%d", param->init.status);
        }
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT status:%d handle:%"PRIu32" close_by_remote:%d", param->close.status,
                 param->close.handle, param->close.async);
        break;
    case ESP_SPP_START_EVT:
        if (param->start.status == ESP_SPP_SUCCESS) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT handle:%"PRIu32" sec_id:%d scn:%d", param->start.handle, param->start.sec_id,
                     param->start.scn);
            esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            connectionHandle = param->start.handle;
        } else {
            ESP_LOGE(SPP_TAG, "ESP_SPP_START_EVT status:%d", param->start.status);
        }
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:

        recevoirMessage(param->data_ind.data, param->data_ind.len);
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        /*
         * We only show the data in which the data length is less than 128 here. If you want to print the data and
         * the data rate is high, it is strongly recommended to process them in other lower priority application task
         * rather than in this callback directly. Since the printing takes too much time, it may stuck the Bluetooth
         * stack and also have a effect on the throughput!
         */
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len:%d handle:%lud",
                 param->data_ind.len, param->data_ind.handle);
        if (param->data_ind.len < 128) {
            esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);
        }
#else
        gettimeofday(&time_new, NULL);
        data_num += param->data_ind.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT status:%d handle:%"PRIu32", rem_bda:[%s]", param->srv_open.status,
                 param->srv_open.handle, bda2str(param->srv_open.rem_bda, bda_str, sizeof(bda_str)));
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    char bda_str[18] = {0};

    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s bda:[%s]", param->auth_cmpl.device_name,
                     bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)));
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %"PRIu32, param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%"PRIu32, param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d bda:[%s]", param->mode_chg.mode,
                 bda2str(param->mode_chg.bda, bda_str, sizeof(bda_str)));
        break;

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

void initBT()
{
    char bda_str[18] = {0};
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    
#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

    ESP_LOGI(SPP_TAG, "Own address:[%s]", bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
}
