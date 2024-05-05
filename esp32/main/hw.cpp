#include "hw.h"
#include "defs.h"
#include "display.h"

#include "esp_log.h"

#include <freertos/FreeRTOS.h>
#include <driver/i2c.h>

// 1001000
const int i2c_address = 0x48;
const int SINGLE_ENDED = 0x80;
const int IREF_OFF_AD_ON = 0x04; // Internal reference OFF, A/D converter ON

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

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = PIN_SDA;
    conf.scl_io_num = PIN_SCL;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

int read_adc(int channel)
{
    channel = channel & 0x7;
    const uint8_t csel = (channel >> 1) | ((channel & 0x1) << 2);
    const uint8_t data = SINGLE_ENDED | csel << 4 | IREF_OFF_AD_ON;
  
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, i2c_address << 1 | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, data, 1);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    if (ret == ESP_ERR_TIMEOUT)
        printf("Error [volume]: Bus is busy\n");
    else if (ret != ESP_OK)
        printf("Error [volume]: Write failed: %d", ret);
    i2c_cmd_link_delete(cmd);

    return 0;
}

// Variables:
// compile-command: "cd .. && idf.py build"
// End:
