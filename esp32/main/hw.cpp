#include "hw.h"
#include "defs.h"
#include "display.h"

#include "esp_log.h"

#include <freertos/FreeRTOS.h>
#include <driver/i2c_master.h>

// 1001000
const int i2c_address = 0x48;
const int SINGLE_ENDED = 0x80;
const int IREF_OFF_AD_ON = 0x04; // Internal reference OFF, A/D converter ON

i2c_master_bus_handle_t i2c_bus_handle;

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
        .i2c_port = -1,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        //trans_queue_depth = 
    };
    i2c_mst_config.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &i2c_bus_handle));
}

int read_adc(int channel)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_address,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &dev_handle));
    channel = channel & 0x7;
    const uint8_t csel = (channel >> 1) | ((channel & 0x1) << 2);
    const uint8_t data = SINGLE_ENDED | csel << 4 | IREF_OFF_AD_ON;
    uint8_t value[2];
    auto res = i2c_master_transmit_receive(dev_handle, &data, 1, value, 2, 100);
    ESP_ERROR_CHECK(res);
    //printf("i2c: %d\n", res);
    
    return value[0] + value[1] * 256;
}

// Variables:
// compile-command: "cd .. && idf.py build"
// End:
