#include "nvs.h"

#include "defs.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nvs_flash.h"

const int MAX_STICK = 4;

static uint16_t stick_calibration[2 * MAX_STICK];

void set_stick_calibration(int stick, int min_val, int max_val)
{
    stick_calibration[stick * 2] = min_val;
    stick_calibration[stick * 2 + 1] = max_val;
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_blob(my_handle, STICK_KEY, stick_calibration, sizeof(stick_calibration)));
    nvs_close(my_handle);
}

const uint16_t* get_stick_calibration(int stick)
{
    return &(stick_calibration[stick * 2]);
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
            stick_calibration[i] = 0;
            stick_calibration[i + 1] = 4095;
        }
    }
    else
    {
        printf("Stick calibration data:\n");
        for (int i = 0; i < MAX_STICK; ++i)
            printf("%d  %5d  %5d\n", i,
                   stick_calibration[i * 2],
                   stick_calibration[i * 2 + 1]);
    }
    nvs_close(my_handle);
}
