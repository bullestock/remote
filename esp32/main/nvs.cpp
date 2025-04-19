#include "nvs.h"

#include "defs.h"
#include "lowpass.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_now.h"
#include "nvs_flash.h"

const int MAX_STICK = 4;

static calibration_data stick_calibration[MAX_STICK];
static char peer_mac[2*ESP_NOW_ETH_ALEN + 1];
static float lowpass_rate = 0.0;

void set_stick_calibration(int stick,
                           int min_val,
                           int mid_val,
                           int max_val)
{
    stick_calibration[stick].min = min_val;
    stick_calibration[stick].mid = mid_val;
    stick_calibration[stick].max = max_val;
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_blob(my_handle, STICK_KEY, stick_calibration, sizeof(stick_calibration)));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

bool set_peer_mac(const char* mac)
{
    if (strlen(mac) != 2*ESP_NOW_ETH_ALEN)
        return false;
    strcpy(peer_mac, mac);
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_str(my_handle, MAC_KEY, peer_mac));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
    return true;
}

bool set_lowpass_rate(float rate)
{
    if (rate <= 0.0 || rate >= 1.0)
        return false;
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_blob(my_handle, LP_RATE_KEY, &rate, sizeof(rate)));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
    return true;
}

const calibration_data& get_stick_calibration(int stick)
{
    return stick_calibration[stick];
}

const char* get_peer_mac()
{
    return peer_mac;
}

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    size_t sz = sizeof(stick_calibration);
    if (nvs_get_blob(my_handle, STICK_KEY, stick_calibration, &sz) != ESP_OK ||
        sz != sizeof(stick_calibration))
    {
        printf("No stick calibration data\n");
        for (int i = 0; i < MAX_STICK; ++i)
        {
            stick_calibration[i].min = 0;
            stick_calibration[i].mid = 2048;
            stick_calibration[i].max = 4095;
        }
    }
    else
    {
        printf("Stick calibration data:\n");
        for (int i = 0; i < MAX_STICK; ++i)
            printf("%d  %5d  %5d  %5d\n", i,
                   stick_calibration[i].min,
                   stick_calibration[i].mid,
                   stick_calibration[i].max);
    }
    sz = 0;
    if (nvs_get_str(my_handle, MAC_KEY, nullptr, &sz) != ESP_OK ||
        sz != 2*ESP_NOW_ETH_ALEN + 1)
    {
        printf("No peer MAC (%d)\n", (int) sz);
        peer_mac[0] = 0;
    }
    else
    {
        if (nvs_get_str(my_handle, MAC_KEY, peer_mac, &sz) != ESP_OK)
        {
            printf("Error reading peer MAC\n");
            peer_mac[0] = 0;
        }
        else
            printf("Peer MAC %s\n", peer_mac);
    }
    sz = sizeof(lowpass_rate);
    if (nvs_get_blob(my_handle, LP_RATE_KEY, &lowpass_rate, &sz) != ESP_OK ||
        sz != sizeof(lowpass_rate))
        printf("No lowpass rate\n");
    else
    {
        printf("Lowpass rate: %f\n", lowpass_rate);
        LowPassFilter::set_rate(lowpass_rate);
    }
    nvs_close(my_handle);
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
