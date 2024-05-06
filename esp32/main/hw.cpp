#include "hw.h"
#include "defs.h"
#include "display.h"

#include "esp_log.h"

#include <freertos/FreeRTOS.h>
#include <driver/i2c_master.h>

// 1001000
const int ADC_ADDRESS = 0x48;
const int SINGLE_ENDED = 0x80;
const int IREF_ON_AD_ON = 0x0C; // Internal reference ON, A/D converter ON

i2c_master_dev_handle_t adc_handle;

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

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADC_ADDRESS,
        .scl_speed_hz = 100000,
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &adc_handle));
}

int read_adc(int channel)
{
    channel = channel & 0x7;
    //const uint8_t csel = (channel >> 1) | ((channel & 0x1) << 2);
    const uint8_t data = SINGLE_ENDED | channel << 4 | IREF_ON_AD_ON;
    uint8_t value[2];
    auto res = i2c_master_transmit_receive(adc_handle, &data, 1, value, 2, 100);
    ESP_ERROR_CHECK(res);
    //printf("i2c: %d\n", res);
    
    return value[0] + value[1] * 256;
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
