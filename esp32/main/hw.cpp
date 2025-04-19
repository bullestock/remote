#include "hw.h"
#include "defs.h"
#include "display.h"
#include "lowpass.h"
#include "nvs.h"
#include "protocol.h"

#include <esp_log.h>

#include <freertos/FreeRTOS.h>

#include <driver/i2c_master.h>

#include <unistd.h>

#include <cmath>

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
    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading ADC: %d", res);
        return std::numeric_limits<int>::max();
    }

    return value[0] * 256 + value[1];
}

void read_switches(ForwardAirFrame& frame)
{
    uint16_t tmp = 0;

    gpio_set_level(PIN_CP, 1);
    // Load data into registers
    gpio_set_level(PIN_PL, 0);
    usleep(1);
    // Hold data
    gpio_set_level(PIN_PL, 1);
    gpio_set_level(PIN_CP, 0);
    for (int i = 0; i < 16; ++i)
    {
        // Shift data
        gpio_set_level(PIN_CP, 1);
        usleep(1);
        gpio_set_level(PIN_CP, 0);
        usleep(1);
        // Read one bit
        auto val = gpio_get_level(PIN_Q7);
        if (!val)
            tmp |= (1 << i);
    }
    
    // tmp now contains:
    //
    // 1111110000000000
    // 5432109876543210
    // ST3T4T1T2PPP PPP
    // 0        654 321

    const uint8_t pushbuttons = tmp & 0x77;
    frame.pushbuttons = ((pushbuttons & 0x70) >> 1) + (pushbuttons & 0x07);

    frame.slide = (tmp & 0x0008) >> 3;

    // Get toggle bits
    tmp = (tmp & 0x7F80) >> 7;
    frame.toggles =
        ((tmp & 0x40) >> 1) +
        ((tmp & 0x80) >> 3) +
        ((tmp & 0x10) << 3) +
        ((tmp & 0x20) << 1) +
        ((tmp & 0x0C) >> 2) +
        ((tmp & 0x03) << 2);
}

static float clamped(int value, float min_value = -1.0)
{
    if (value < 0)
        return min_value;
    if (value > 4095)
        return 1.0;
    return (value - 2048)/2048.0;
}

static float calibrated(int stick, int value)
{
    if (value >= 4096)
        return NAN;
    const auto cal = get_stick_calibration(stick);
    if (cal.max == 0)
        return clamped(value);
    if (value >= cal.mid)
        return clamped(2048 + (value - cal.mid) * 2048 / (cal.max - cal.mid));
    return clamped((value - cal.min) * 2048 / (cal.mid - cal.min));
}

bool check(float value)
{
    return value >= -1.0 && value <= 1.0;
}

bool check(const ForwardAirFrame& frame)
{
    return check(frame.left_x) &&
        check(frame.left_y) &&
        check(frame.right_x) &&
        check(frame.right_y) &&
        check(frame.left_pot) &&
        check(frame.right_pot);
}

static LowPassFilter filters[4];

float read_stick(int stick, bool initial)
{
    int chan = 0;
    int sign = 1;
    switch (stick)
    {
    case 0:
        chan = LEFT_X_CHANNEL;
        break;
    case 1:
        chan = LEFT_Y_CHANNEL;
        sign = -1;
        break;
    case 2:
        chan = RIGHT_X_CHANNEL;
        sign = -1;
        break;
    case 3:
        chan = RIGHT_Y_CHANNEL;
        break;
    }

    return sign * calibrated(stick, filters[stick].filter(read_adc(chan), initial));
}

bool fill_frame(ForwardAirFrame& frame,
                int64_t ticks,
                bool initial)
{
    frame.magic = ForwardAirFrame::MAGIC_VALUE;
    frame.ticks = ticks;
    frame.left_x = read_stick(0, initial);
    frame.left_y = read_stick(1, initial);
    frame.right_x = read_stick(2, initial);
    frame.right_y = read_stick(3, initial);
    frame.left_pot = clamped(read_adc(POT1_CHANNEL), 0.0);
    frame.right_pot = clamped(read_adc(POT2_CHANNEL), 0.0);
    if (!check(frame))
        return false;
    read_switches(frame);
    set_crc(frame);
    return true;
}

float get_my_battery()
{
    const int N = 10;
    static int samples[N];
    static int nof_samples = 0;

    // Initial fill
    while (nof_samples < N)
        samples[nof_samples++] = read_adc(BATTERY_CHANNEL);

    // Shift
    for (int i = 1; i < N; ++i)
        samples[i - 1] = samples[i];

    // Add new sample
    samples[N - 1] = read_adc(BATTERY_CHANNEL);

    // Compute average
    int64_t my_battery = 0;
    for (int i = 1; i < N; ++i)
        my_battery += samples[i];

    // Full scale is 2^12
    // Resistors divide by 2
    // Reference voltage is 2.5 V
    const auto fudge = 1.125;
    return static_cast<float>(my_battery)/(N * 4096.0/2.0/2.5) * fudge;
}

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:
