#include "console.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "nvs.h"
#include "radio.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

extern "C"
void app_main(void)
{
    printf("Remote v %s\n", VERSION);

    init_hardware();

    SSD1306_t ssd;
    Display display(ssd);

    NRF24_t nrf24;
    bool debug = false;
    if (init_radio(nrf24))
        printf("nRF24 init OK\n");
    else
    {
        printf("nRF24 init failed!\n");
        display.add_progress("ERROR: No radio");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        debug = true;
    }

    init_nvs();
    
    if (!debug)
    {
        printf("\n\nPress a key to enter console\n");
        display.add_progress("Console wait");
        for (int i = 0; i < 20; ++i)
        {
            if (getchar() != EOF)
            {
                debug = true;
                break;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    if (debug)
        run_console(display, nrf24);        // never returns

    printf("\nStarting application\n");

    int consecutive_errors = 0;
    unsigned long failures = 0;
    unsigned long crc_errors = 0;
    unsigned long successes = 0;
    bool show_error = false;
    uint16_t their_battery = 0;

    const int NOF_DELAY_SAMPLES = 10;
    int64_t delay_samples[NOF_DELAY_SAMPLES];
    int actual_delay_samples = 0;

    while (1)
    {
        const auto send_time = esp_timer_get_time();
        ForwardAirFrame frame;
        fill_frame(frame, send_time);

        const auto my_battery = get_my_battery();

#ifdef LIVE_VIEW
        display.clear();
        display.add_progress(format("L %3d %3d",
                                    frame.left_x, frame.left_y));
        display.add_progress(format("R %3d %3d",
                                    frame.right_x, frame.right_y));
        display.add_progress(format("B %02X T %02X S %d",
                                    frame.pushbuttons,
                                    frame.toggles,
                                    frame.slide));
        display.add_progress(format("P %02X %02X",
                                    frame.left_pot, frame.right_pot));
        display.add_progress(format("B %.2f", my_battery));
        vTaskDelay(500 / portTICK_PERIOD_MS);
#else
        // Normal operation
        //display.add_progress(format("%d %d", successes, failures));
        
        bool timeout = false;
        bool ok = send_frame(nrf24, frame, timeout);
        if (ok)
            consecutive_errors = 0;
        else
            ++consecutive_errors;

        if (timeout)
            ++failures;

        bool show_error = (consecutive_errors > 10);

        if (ok)
        {
            ++successes;

            uint8_t data[sizeof(ForwardAirFrame)];
            Nrf24_getData(&nrf24, data);
            ReturnAirFrame ret_frame;
            memcpy(data, &ret_frame, sizeof(ret_frame));
            if ((ret_frame.magic == ReturnAirFrame::MAGIC_VALUE) &&
                check_crc(ret_frame))
            {
                const auto end_time = esp_timer_get_time();

                for (int i = actual_delay_samples-1; i > 0; --i)
                    delay_samples[i] = delay_samples[i-1];
                delay_samples[0] = end_time - send_time;

                their_battery = ret_frame.battery;
            }
            else
            {
                ++crc_errors;
                show_error = true;
            }
        }
#endif        
    }
}
