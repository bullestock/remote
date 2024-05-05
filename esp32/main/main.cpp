#include "console.h"
#include "defs.h"
#include "display.h"
#include "hw.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C"
void app_main(void)
{
    printf("Remote v %s\n", VERSION);

    init_hardware();

    Display display;

    printf("\n\nPress a key to enter console\n");
    bool debug = false;
    for (int i = 0; i < 20; ++i)
    {
        if (getchar() != EOF)
        {
            debug = true;
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    if (debug)
        run_console(display);        // never returns

    printf("\nStarting application\n");

    while (1)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}
