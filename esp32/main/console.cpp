#include "console.h"
#include "defs.h"
#include "display.h"
#include "hw.h"
#include "nvs.h"

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
static NRF24_t* the_radio = nullptr;

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
        printf("ADC channel %d: %d\n", chan, read_adc(chan));
    }
    
    return 0;
}

static int test_display(int, char**)
{
    printf("Running display test\n");

    ssd1306_display_text(the_display->device(), 0, "Hello", 5, false);
    
    return 0;
}

#if 0

static int test_i2c(int, char**)
{
    printf("Scanning for I2C devices\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:         ");
    for (int i = 3; i < 0x78; ++i)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_stop(cmd);

        const auto espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
        if (i%16 == 0)
            printf("\n%.2x:", i);
        if (espRc == 0)
            printf(" %.2x", i);
        else
            printf(" --");
        i2c_cmd_link_delete(cmd);
    }
    printf("\n");
    
    return 0;
}

#endif

static const char* yes_no(bool b)
{
    return b ? "yes" : "no";
}

static int test_radio(int, char**)
{
    printf("Running radio test\n");

    ForwardAirFrame frame;
    bool timeout = false;
    const auto send_time = esp_timer_get_time();
    bool ok = send_frame(*the_radio, send_time, frame, timeout);

    if (!ok)
    {
        printf("send failed\n");
        return 1;
    }
    
    uint8_t data[sizeof(ForwardAirFrame)];
    memset(data, 0, sizeof(data));
    Nrf24_getData(the_radio, data);
    ReturnAirFrame ret_frame;
    memcpy(data, &ret_frame, sizeof(ret_frame));
    for (auto b : data)
        printf("%02X ", b);
    printf("\nReturn frame magic value OK: %s   CRC OK: %s\n",
           yes_no(ret_frame.magic == ReturnAirFrame::MAGIC_VALUE),
           yes_no(check_crc(ret_frame)));

    return 0;
}

static int test_switches(int, char**)
{
    printf("Running switch test\n");

    for (int n = 0; n < 10; ++n)
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        ForwardAirFrame frame;
        read_switches(frame);
        printf("Toggles: %02X  Push: %02X  Slide: %01X\n",
               frame.toggles, frame.pushbuttons, frame.slide);
    }
    
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

void run_console(Display& display,
                 NRF24_t& radio)
{
    the_display = &display;
    the_radio = &radio;
    
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
    
    test_adc_args.channel = arg_int1(NULL, NULL, "<channel>", "Channel");
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

#if 0
    const esp_console_cmd_t test_i2c_cmd = {
        .command = "i2c",
        .help = "Test I2C",
        .hint = nullptr,
        .func = &test_i2c,
        .argtable = nullptr
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&test_i2c_cmd));
#endif

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
        prompt = "esp32> ";
#endif //CONFIG_LOG_COLORS
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
