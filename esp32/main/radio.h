#pragma once

#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_now.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   (wifi_interface_t) ESP_IF_WIFI_STA

#define ESPNOW_QUEUE_SIZE           6

enum espnow_event_id_t {
    ESPNOW_SEND_CB,
    ESPNOW_RECV_CB,
};

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} espnow_event_recv_cb_t;

union espnow_event_info_t {
    esp_now_send_status_t send_status;
    espnow_event_recv_cb_t recv_cb;
};

/* When ESPNOW sending or receiving callback function is called, post event to ESPNOW task. */
struct espnow_event_t {
    espnow_event_id_t id;
    espnow_event_info_t info;
};

class ForwardAirFrame;

bool init_radio();

bool send_frame(ForwardAirFrame& frame);

bool data_ready();


