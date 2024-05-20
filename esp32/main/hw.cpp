#include "hw.h"
#include "defs.h"
#include "display.h"

#include "esp_log.h"

#include <freertos/FreeRTOS.h>

#include <driver/i2c_master.h>

#include <unistd.h>

// 1001000
const int SINGLE_ENDED = 0x80;
const int IREF_ON_AD_ON = 0x0C; // Internal reference ON, A/D converter ON
const int I2C_MASTER_FREQ_HZ = 400000; // I2C clock of SSD1306 can run at 400 kHz max.

i2c_master_dev_handle_t adc_handle;

extern i2c_master_dev_handle_t dev_handle;

void init_hardware()
{
    // Inputs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    // bit mask of the pins that you want to set
    io_conf.pin_bit_mask = 
       (1ULL << PIN_MISO) |
       (1ULL << PIN_Q7);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // Outputs
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set
    io_conf.pin_bit_mask = 
       (1ULL << PIN_PL) |
       (1ULL << PIN_CP) |
       (1ULL << PIN_CS) |
       (1ULL << PIN_CE) |
       (1ULL << PIN_SCK) |
       (1ULL << PIN_MOSI);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        //trans_queue_depth = 
    };
    i2c_mst_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t i2c_bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &i2c_bus_handle));

    i2c_device_config_t adc_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADC_I2C_ADDRESS,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &adc_cfg, &adc_handle));

    i2c_device_config_t display_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = I2C_ADDRESS,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &display_cfg, &dev_handle));
}

int read_adc(int channel)
{
    channel = channel & 0x7;
    const uint8_t data = SINGLE_ENDED | channel << 4 | IREF_ON_AD_ON;
    uint8_t value[2];
    auto res = i2c_master_transmit_receive(adc_handle, &data, 1, value, 2, 100);
    ESP_ERROR_CHECK(res);
    
    return value[0] * 256 + value[1];
}

void read_switches(ForwardAirFrame& frame)
{
    uint16_t tmp = 0;

    gpio_set_level(PIN_CP, 1);
    gpio_set_level(PIN_PL, 0);
    usleep(5);
    gpio_set_level(PIN_PL, 1);
    gpio_set_level(PIN_CP, 0);
    for (int i = 0; i < 16; ++i)
    {
        auto val = gpio_get_level(PIN_Q7);
        if (!val)
            tmp |= (1 << i);
        gpio_set_level(PIN_CP, 1);
        usleep(5);
        gpio_set_level(PIN_CP, 0);
        usleep(5);
    }
    
    // tmp now contains:
    //
    // 1111110000000000
    // 5432109876543210
    // T3T4T1T2PPPPPPS1
    //         456123

    // Swap P4 with P6 and P1 with P3
    uint8_t pushbuttons = (tmp & 0xFC) >> 2;
    frame.pushbuttons = (pushbuttons & 0x12)
       + (pushbuttons & 0x20 ? 0x08 : 0)
       + (pushbuttons & 0x08 ? 0x20 : 0)
       + (pushbuttons & 0x04 ? 0x01 : 0)
       + (pushbuttons & 0x01 ? 0x04 : 0);

    // Swap T3 with T4 and T1 with T2
    uint16_t toggles = (tmp & 0xFF00) >> 8;
    frame.toggles =
       ((toggles & 0xC0) >> 2) +
       ((toggles & 0x30) << 2) +
       ((toggles & 0x0C) >> 2) +
       ((toggles & 0x03) << 2);
    
    frame.slide = tmp & 0x03;
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
