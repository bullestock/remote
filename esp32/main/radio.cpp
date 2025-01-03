#include "defs.h"
#include "hw.h"
#include "nvs.h"
#include "protocol.h"
#include "radio.h"

#include "esp_crc.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_random.h"

#include <string.h>
#include <stdint.h>

// Ticks to wait for queue to become available
#define ESPNOW_MAXDELAY 512

static SemaphoreHandle_t send_mutex;
static bool send_pending = false; // protected by send_mutex

SemaphoreHandle_t receive_mutex;
bool data_ready = false;       // protected by receive_mutex
ReturnAirFrame received_frame; // protected by receive_mutex

static QueueHandle_t s_espnow_queue;

static uint8_t s_other_mac[ESP_NOW_ETH_ALEN];

static void espnow_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status);
static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
static void espnow_task(void *pvParameter);

void fatal_error(const char* why)
{
    printf("FATAL ERROR: %s\n", why);
    esp_restart();
}

static unsigned int to_int(char c)
{
  if (c >= '0' && c <= '9')
      return c - '0';
  if (c >= 'A' && c <= 'F')
      return 10 + c - 'A';
  if (c >= 'a' && c <= 'f')
      return 10 + c - 'a';
  return -1;
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

    uint8_t mac[ESP_NOW_ETH_ALEN];
    esp_wifi_get_mac((wifi_interface_t) ESP_IF_WIFI_STA, mac);
    ESP_LOGI(TAG, "My MAC: " MACSTR, MAC2STR(mac));

    const auto peer_mac = get_peer_mac();
    for (size_t i = 0; i < ESP_NOW_ETH_ALEN; ++i)
        s_other_mac[i] = 16 * to_int(peer_mac[2*i]) + to_int(peer_mac[2*i+1]);
    ESP_LOGI(TAG, "Other MAC: " MACSTR, MAC2STR(s_other_mac));

    s_espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));
    if (s_espnow_queue == NULL) {
        ESP_LOGE(TAG, "Create queue fail");
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

    // Add peer to peer list
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

    send_mutex = xSemaphoreCreateMutex();
    receive_mutex = xSemaphoreCreateMutex();

    // Allocate first frame
    auto frame = reinterpret_cast<ForwardAirFrame*>(malloc(sizeof(ForwardAirFrame)));
    if (!frame)
        fatal_error("Malloc send parameter fail");

    set_crc(*frame);

    xTaskCreate(espnow_task, "espnow_task", 4096, frame, 4, NULL);
    
    return true;
}

// ESPNOW sending or receiving callback function is called in WiFi task.
// Users should not do lengthy operations from this task. Instead, post
// necessary data to a queue and handle it from a lower priority task.
static void espnow_send_cb(const uint8_t*, esp_now_send_status_t status)
{
    espnow_event_t evt;
    evt.id = ESPNOW_SEND_CB;
    evt.info.send_status = status;
    if (xQueueSend(s_espnow_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE)
        ESP_LOGW(TAG, "Send send queue fail");
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

static void espnow_task(void *pvParameter)
{
    espnow_event_t evt;

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Start sending data");

    auto frame = (ForwardAirFrame*) pvParameter;
    ESP_ERROR_CHECK(esp_now_send(s_other_mac, (uint8_t*) frame, sizeof(ForwardAirFrame)));

    while (xQueueReceive(s_espnow_queue, &evt, portMAX_DELAY) == pdTRUE)
    {
        switch (evt.id)
            {
            case ESPNOW_SEND_CB:
            {
                const auto send_status = evt.info.send_status;

                ESP_LOGD(TAG, "Sent data, status: %d", send_status);

                if (xSemaphoreTake(send_mutex, portMAX_DELAY) == pdTRUE)
                {
                    send_pending = false;
                    xSemaphoreGive(send_mutex);
                }
                break;
            }
            case ESPNOW_RECV_CB:
            {
                const auto recv_cb = &evt.info.recv_cb;

                if (recv_cb->data_len < sizeof(ReturnAirFrame))
                    ESP_LOGE(TAG, "Received ESPNOW data too short, len:%d", recv_cb->data_len);
                else
                {
                    auto frame = reinterpret_cast<const ReturnAirFrame*>(recv_cb->data);
                    if (xSemaphoreTake(receive_mutex, portMAX_DELAY) == pdTRUE)
                    {
                        if (!data_ready)
                        {
                            data_ready = true;
                            memcpy(&received_frame, frame, sizeof(ReturnAirFrame));
                        }
                        xSemaphoreGive(receive_mutex);
                    }
                }
                free(recv_cb->data);
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

    if (xSemaphoreTake(send_mutex, portMAX_DELAY) == pdTRUE)
    {
        if (send_pending)
        {
            // A frame is already pending.
            xSemaphoreGive(send_mutex);
            return true; //false;
        }
        send_pending = true;
        xSemaphoreGive(send_mutex);
    }
    return esp_now_send(s_other_mac, (uint8_t*) &frame, sizeof(ForwardAirFrame)) == ESP_OK;
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
