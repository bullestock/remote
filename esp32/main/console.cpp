#include "console.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "nvs.h"
#include "protocol.h"

#include <string>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include <esp_timer.h>
#include "esp_vfs_dev.h"

#include <driver/uart.h>

#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

static Display* the_display = nullptr;

static int reboot(int, char**)
{
    printf("Reboot...\n");
    esp_restart();
    return 0;
}

struct
{
    struct arg_int* channel;
    struct arg_end* end;
} test_adc_args;

static int test_adc(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &test_adc_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, test_adc_args.end, argv[0]);
        return 1;
    }
    printf("Running ADC test\n");

    const int chan = test_adc_args.channel->ival[0];
    for (int n = 0; n < 200; ++n)
    {
        vTaskDelay(50/portTICK_PERIOD_MS);
        if (chan < 0)
        {
            for (int i = 0; i < 7; ++i)
            {
                for (int j = -1; j > chan; --j)
                    read_adc(i);
                printf("%04d ", read_adc(i));
            }
            printf("\n");
        }
        else
            printf("ADC channel %d: %d\n", chan, read_adc(chan));
    }
    
    return 0;
}

static int test_display(int, char**)
{
    printf("Running display test\n");

    the_display->clear();
    for (int i = 0; i < 20; ++i)
    {
        the_display->add_progress(format("Line %d", i+1));
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
    return 0;
}

struct
{
    struct arg_int* stick;
    struct arg_int* min_val;
    struct arg_int* max_val;
    struct arg_end* end;
} calibrate_args;

static int calibrate(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &calibrate_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, calibrate_args.end, argv[0]);
        return 1;
    }

    if (calibrate_args.stick->ival[0] < 0 || calibrate_args.stick->ival[0] > 3 ||
        calibrate_args.min_val->ival[0] < 0 || calibrate_args.min_val->ival[0] > 4096 ||
        calibrate_args.max_val->ival[0] < 0 || calibrate_args.max_val->ival[0] > 4096)
    {
        printf("Invalid arguments\n");
        return 1;
    }

    set_stick_calibration(calibrate_args.stick->ival[0],
                          calibrate_args.min_val->ival[0],
                          calibrate_args.max_val->ival[0]);    
    return 0;
}

static int test_radio(int, char**)
{
    printf("Radio test N/A\n");
    return 0;
}

static int test_switches(int, char**)
{
    printf("Running switch test\n");

    for (int n = 0; n < 25; ++n)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        ForwardAirFrame frame;
        read_switches(frame);
        printf("Toggles: %02X  Push: %02X  Slide: %01X\n",
               frame.toggles, frame.pushbuttons, frame.slide);
    }
    
    return 0;
}

struct
{
    struct arg_str* peer_mac;
    struct arg_end* end;
} set_peer_mac_args;

int set_peer_mac(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &set_peer_mac_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, set_peer_mac_args.end, argv[0]);
        return 1;
    }
    const auto peer_mac = set_peer_mac_args.peer_mac->sval[0];
    if (!set_peer_mac(peer_mac))
    {
        printf("ERROR: Invalid MAC\n");
        return 1;
    }
    printf("OK: Peer MAC set to %s\n", peer_mac);
    return 0;
}

void initialize_console()
{
    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(0, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(0, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    uart_config_t uart_config;
    memset(&uart_config, 0, sizeof(uart_config));
    uart_config.baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.source_clk = UART_SCLK_REF_TICK;
    ESP_ERROR_CHECK(uart_param_config((uart_port_t) CONFIG_ESP_CONSOLE_UART_NUM, &uart_config));

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK(uart_driver_install((uart_port_t) CONFIG_ESP_CONSOLE_UART_NUM,
                                         256, 0, 0, NULL, 0));

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config;
    memset(&console_config, 0, sizeof(console_config));
    console_config.max_cmdline_args = 8;
    console_config.max_cmdline_length = 256;
#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#endif
    ESP_ERROR_CHECK(esp_console_init(&console_config));

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);
}

void run_console(Display& display)
{
    the_display = &display;
    
    initialize_console();

    esp_console_register_help_command();

    const esp_console_cmd_t reboot_cmd = {
        .command = "reboot",
        .help = "Reboot",
        .hint = nullptr,
        .func = &reboot,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reboot_cmd));
    
    test_adc_args.channel = arg_int1(NULL, NULL, "<channel>", "Channel (-1 for all)");
    test_adc_args.end = arg_end(2);
    const esp_console_cmd_t test_adc_cmd = {
        .command = "adc",
        .help = "Test ADC",
        .hint = nullptr,
        .func = &test_adc,
        .argtable = &test_adc_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_adc_cmd));

    const esp_console_cmd_t test_display_cmd = {
        .command = "display",
        .help = "Test display",
        .hint = nullptr,
        .func = &test_display,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_display_cmd));

    calibrate_args.stick = arg_int1(NULL, NULL, "<stick>", "Stick (0: LX, 1: LY, 2: RX, 3: RY)");
    calibrate_args.min_val = arg_int1(NULL, NULL, "<min>", "Minimum value (0-4095)");
    calibrate_args.max_val = arg_int1(NULL, NULL, "<min>", "Minimum value (0-4095)");
    calibrate_args.end = arg_end(2);
    const esp_console_cmd_t calibrate_cmd = {
        .command = "cal",
        .help = "Calibrate sticks",
        .hint = nullptr,
        .func = &calibrate,
        .argtable = &calibrate_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&calibrate_cmd));

    const esp_console_cmd_t test_radio_cmd = {
        .command = "radio",
        .help = "Test radio",
        .hint = nullptr,
        .func = &test_radio,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_radio_cmd));

    const esp_console_cmd_t test_switches_cmd = {
        .command = "switches",
        .help = "Test switches",
        .hint = nullptr,
        .func = &test_switches,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_switches_cmd));
    
    set_peer_mac_args.peer_mac = arg_str1(NULL, NULL, "<mac>", "Peer MAC");
    set_peer_mac_args.end = arg_end(2);
    const esp_console_cmd_t set_peer_mac_cmd = {
        .command = "mac",
        .help = "Set peer MAC",
        .hint = nullptr,
        .func = &set_peer_mac,
        .argtable = &set_peer_mac_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_peer_mac_cmd));

    const char* prompt = LOG_COLOR_I "remote> " LOG_RESET_COLOR;
    int probe_status = linenoiseProbe();
    if (probe_status)
    {
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the prompt.
         */
        prompt = "remote> ";
#endif // CONFIG_LOG_COLORS
    }

    while (true)
    {
        char* line = linenoise(prompt);
        if (!line)
            continue;

        linenoiseHistoryAdd(line);

        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND)
            printf("Unrecognized command\n");
        else if (err == ESP_ERR_INVALID_ARG)
            ; // command was empty
        else if (err == ESP_OK && ret != ESP_OK)
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(err));
        else if (err != ESP_OK)
            printf("Internal error: %s\n", esp_err_to_name(err));

        linenoiseFree(line);
    }
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
