#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "sh1107.h"

#define TAG "SH1107"

#define I2C_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000 /*!< I2C master clock frequency. no higher than 1MHz for now */
#define I2C_TICKS_TO_WAIT 100     // Maximum ticks to wait before issuing a timeout.

i2c_master_dev_handle_t dev_handle;

void i2c_master_init(SH1107_t * dev, int16_t sda, int16_t scl, int16_t reset)
{
	ESP_LOGI(TAG, "New i2c driver is used");
	i2c_master_bus_config_t i2c_mst_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.i2c_port = I2C_NUM,
		.scl_io_num = scl,
		.sda_io_num = sda,
		.flags.enable_internal_pullup = true,
	};
	i2c_master_bus_handle_t bus_handle;
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = I2C_ADDRESS,
		.scl_speed_hz = I2C_MASTER_FREQ_HZ,
	};
	//i2c_master_dev_handle_t dev_handle;
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

	if (reset >= 0) {
		//gpio_pad_select_gpio(reset);
		gpio_reset_pin(reset);
		gpio_set_direction(reset, GPIO_MODE_OUTPUT);
		gpio_set_level(reset, 0);
		vTaskDelay(50 / portTICK_PERIOD_MS);
		gpio_set_level(reset, 1);
	}
	dev->_address = I2C_ADDRESS;
}

bool i2c_init(SH1107_t * dev, int width, int height)
{
	dev->_width = width;
	dev->_height = height;
	dev->_pages = height / 8;
	dev->_direction = DIRECTION0;
	
	uint8_t out_buf[27];
	int out_index = 0;
	out_buf[out_index++] = I2C_CONTROL_BYTE_CMD_STREAM;
	out_buf[out_index++] = 0xAE;
	out_buf[out_index++] = 0xDC;
	out_buf[out_index++] = 0x00;
	out_buf[out_index++] = 0x81;
	out_buf[out_index++] = 0x2F;
	out_buf[out_index++] = 0x20;
	out_buf[out_index++] = 0xA0;
	out_buf[out_index++] = 0xC0;
	out_buf[out_index++] = 0xA8;
	out_buf[out_index++] = 0x7F;
	out_buf[out_index++] = 0xD3;
	out_buf[out_index++] = 0x60;
	out_buf[out_index++] = 0xD5;
	out_buf[out_index++] = 0x51;
	out_buf[out_index++] = 0xD9;
	out_buf[out_index++] = 0x22;
	out_buf[out_index++] = 0xDB;
	out_buf[out_index++] = 0x35;
	out_buf[out_index++] = 0xB0;
	out_buf[out_index++] = 0xDA;
	out_buf[out_index++] = 0x12;
	out_buf[out_index++] = 0xA4;
	out_buf[out_index++] = 0xA6;
	//out_buf[out_index++] = 0xA7;	// Inverted display
	out_buf[out_index++] = 0xAF;

	esp_err_t res;
	res = i2c_master_transmit(dev_handle, out_buf, out_index, I2C_TICKS_TO_WAIT);
	if (res == ESP_OK) {
		ESP_LOGI(TAG, "OLED configured successfully");
        return true;
	} 
    ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d (%s)", dev->_address, I2C_NUM, res, esp_err_to_name(res));
	return false;
}


void i2c_display_image(SH1107_t * dev, int page, int seg, uint8_t * images, int width)
{
	if (page >= dev->_pages) return;
	if (seg >= dev->_width) return;

	uint8_t columLow = seg & 0x0F;
	uint8_t columHigh = (seg >> 4) & 0x0F;

	uint8_t *out_buf;
	out_buf = malloc(width + 1);
	if (out_buf == NULL) {
		ESP_LOGE(TAG, "malloc fail");
		return;
	}
    int out_index = 0;
	out_buf[out_index++] = I2C_CONTROL_BYTE_CMD_STREAM;
	// Set Higher Column Start Address for Page Addressing Mode
	out_buf[out_index++] = 0x10 + columHigh;
	// Set Lower Column Start Address for Page Addressing Mode
	out_buf[out_index++] = 0x00 + columLow;
	// Set Page Start Address for Page Addressing Mode
	out_buf[out_index++] = 0xB0 | page;

	esp_err_t res;
	res = i2c_master_transmit(dev_handle, out_buf, out_index, I2C_TICKS_TO_WAIT);
	if (res != ESP_OK)
		ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d (%s)", dev->_address, I2C_NUM, res, esp_err_to_name(res));

	out_buf[0] = I2C_CONTROL_BYTE_DATA_STREAM;
	memcpy(&out_buf[1], images, width);

	res = i2c_master_transmit(dev_handle, out_buf, width + 1, I2C_TICKS_TO_WAIT);
	if (res != ESP_OK)
		ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d (%s)", dev->_address, I2C_NUM, res, esp_err_to_name(res));
	free(out_buf);
}

void i2c_contrast(SH1107_t * dev, int contrast)
{
	int _contrast = contrast;
	if (contrast < 0x0) _contrast = 0;
	if (contrast > 0xFF) _contrast = 0xFF;

	uint8_t out_buf[3];
	int out_index = 0;
	out_buf[out_index++] = I2C_CONTROL_BYTE_CMD_STREAM; // 00
	out_buf[out_index++] = 0x81;		// Set Contrast Control Register
	out_buf[out_index++] = _contrast;

	esp_err_t res = i2c_master_transmit(dev_handle, out_buf, 3, I2C_TICKS_TO_WAIT);
	if (res != ESP_OK)
		ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d (%s)", dev->_address, I2C_NUM, res, esp_err_to_name(res));
}
