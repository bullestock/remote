#include "defs.h"
#include "hw.h"
#include "protocol.h"
#include "radio.h"

#include "esp_crc.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_random.h"

#include <string.h>
#include <stdint.h>

// TODO:
// - keys in NVS
// - other MAC in NVS

#define ESPNOW_MAXDELAY 512

static QueueHandle_t s_espnow_queue;

static uint8_t s_other_mac[ESP_NOW_ETH_ALEN] = { 0xc4, 0xde, 0xe2, 0x19, 0x37, 0x3c };

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
static void espnow_data_prepare(espnow_send_param_t *send_param);
static void espnow_task(void *pvParameter);

void fatal_error(const char* why)
{
    printf("FATAL ERROR: %s\n", why);
    esp_restart();
}

bool init_radio()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESPNOW_WIFI_IF,
                                          WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G |
                                          WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
#endif

    s_espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));
    if (s_espnow_queue == NULL) {
        ESP_LOGE(TAG, "Create mutex fail");
        return false;
    }

    /* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));
#if CONFIG_ESPNOW_ENABLE_POWER_SAVE
    ESP_ERROR_CHECK(esp_now_set_wake_window(CONFIG_ESPNOW_WAKE_WINDOW));
    ESP_ERROR_CHECK(esp_wifi_connectionless_module_set_wake_interval(CONFIG_ESPNOW_WAKE_INTERVAL));
#endif
    /* Set primary master key. */
    ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t*) CONFIG_ESPNOW_PMK));

    /* Add broadcast peer information to peer list. */
    auto peer = reinterpret_cast<esp_now_peer_info_t*>(malloc(sizeof(esp_now_peer_info_t)));
    if (!peer)
        fatal_error("Malloc peer information fail");
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_ESPNOW_CHANNEL;
    peer->ifidx = ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, s_other_mac, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);

    /* Initialize sending parameters. */
    auto send_param = reinterpret_cast<espnow_send_param_t*>(malloc(sizeof(espnow_send_param_t)));
    if (!send_param)
        fatal_error("Malloc send parameter fail");

    memset(send_param, 0, sizeof(espnow_send_param_t));
    send_param->buffer = (ForwardAirFrame*) malloc(sizeof(ForwardAirFrame));
    memcpy(send_param->dest_mac, s_other_mac, ESP_NOW_ETH_ALEN);
    espnow_data_prepare(send_param);

    xTaskCreate(espnow_task, "espnow_task", 2048, send_param, 4, NULL);
    
    return true;
}

/* ESPNOW sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */
static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    espnow_event_t evt;
    espnow_event_send_cb_t *send_cb = &evt.info.send_cb;

    if (mac_addr == NULL) {
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }

    evt.id = ESPNOW_SEND_CB;
    memcpy(send_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    send_cb->status = status;
    if (xQueueSend(s_espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Send send queue fail");
    }
}

static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    espnow_event_t evt;
    espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;
    uint8_t * mac_addr = recv_info->src_addr;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }

    evt.id = ESPNOW_RECV_CB;
    memcpy(recv_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    recv_cb->data = reinterpret_cast<uint8_t*>(malloc(len));
    if (!recv_cb->data)
    {
        ESP_LOGE(TAG, "Malloc receive data fail");
        return;
    }
    memcpy(recv_cb->data, data, len);
    recv_cb->data_len = len;
    if (xQueueSend(s_espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE)
    {
        ESP_LOGW(TAG, "Send receive queue fail");
        free(recv_cb->data);
    }
}

/* Parse received ESPNOW data. */
bool espnow_data_check(uint8_t *data, uint16_t data_len)
{
    const auto buf = (ForwardAirFrame*) data;

    if (data_len < sizeof(ForwardAirFrame)) {
        ESP_LOGE(TAG, "Receive ESPNOW data too short, len:%d", data_len);
        return false;
    }

    return check_crc(*buf);
}

/* Prepare ESPNOW data to be sent. */
static void espnow_data_prepare(espnow_send_param_t *send_param)
{
    auto buf = (ForwardAirFrame*) send_param->buffer;

    set_crc(*buf);
}

static void espnow_task(void *pvParameter)
{
    espnow_event_t evt;

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Start sending broadcast data");

    /* Start sending ESPNOW data. */
    espnow_send_param_t *send_param = (espnow_send_param_t *)pvParameter;
    ESP_ERROR_CHECK(esp_now_send(send_param->dest_mac, (uint8_t*) send_param->buffer, sizeof(ForwardAirFrame)));

    while (xQueueReceive(s_espnow_queue, &evt, portMAX_DELAY) == pdTRUE)
    {
        switch (evt.id) {
            case ESPNOW_SEND_CB:
            {
                espnow_event_send_cb_t *send_cb = &evt.info.send_cb;

                ESP_LOGD(TAG, "Send data to " MACSTR ", status1: %d", MAC2STR(send_cb->mac_addr), send_cb->status);

                vTaskDelay(100/portTICK_PERIOD_MS);

                ESP_LOGI(TAG, "send data to " MACSTR "", MAC2STR(send_cb->mac_addr));

                memcpy(send_param->dest_mac, send_cb->mac_addr, ESP_NOW_ETH_ALEN);
                espnow_data_prepare(send_param);

                /* Send the next data after the previous data is sent. */
                ESP_ERROR_CHECK(esp_now_send(send_param->dest_mac, (uint8_t*) send_param->buffer, sizeof(ForwardAirFrame)));
                break;
            }
            case ESPNOW_RECV_CB:
            {
                espnow_event_recv_cb_t *recv_cb = &evt.info.recv_cb;

                auto ret = espnow_data_check(recv_cb->data, recv_cb->data_len);
                free(recv_cb->data);
                if (ret) {
                    ESP_LOGI(TAG, "Receive unicast data from: " MACSTR ", len: %d", MAC2STR(recv_cb->mac_addr), recv_cb->data_len);
                }
                else {
                    ESP_LOGI(TAG, "Receive error data from: " MACSTR "", MAC2STR(recv_cb->mac_addr));
                }
                break;
            }
            default:
                ESP_LOGE(TAG, "Callback type error: %d", evt.id);
                break;
        }
    }
}

bool send_frame(ForwardAirFrame& frame)
{
#if 0
    printf("X %3d Y %3d X %3d Y %3d S %02X %02X %d P %02X %02X\n",
           frame.left_x, frame.left_y, frame.right_x, frame.right_y,
           frame.pushbuttons,
           frame.toggles,
           frame.slide,
           frame.left_pot, frame.right_pot);
#endif
    
    //Nrf24_send(&dev, reinterpret_cast<uint8_t*>(&frame));

    return false;// Nrf24_isSend(&dev, 1000);
}

bool data_ready()
{
    return false; //Nrf24_dataReady(&dev);
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
