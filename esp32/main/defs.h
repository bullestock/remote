#pragma once

#include <driver/gpio.h>

constexpr const char* VERSION = "0.1.0";

// NVS keys
constexpr const char* STICK_KEY = "stk";
constexpr const char* MAC_KEY = "mac";

// Pin definitions
constexpr const auto PIN_MOSI = (gpio_num_t) 2;
constexpr const auto PIN_SCK = (gpio_num_t) 4;
constexpr const auto PIN_PL = (gpio_num_t) 12;
constexpr const auto PIN_CP = (gpio_num_t) 13;
constexpr const auto PIN_Q7 = (gpio_num_t) 14;
constexpr const auto PIN_CS = (gpio_num_t) 16;
constexpr const auto PIN_CE = (gpio_num_t) 17;
constexpr const auto PIN_SDA = (gpio_num_t) 21;
constexpr const auto PIN_SCL = (gpio_num_t) 22;
constexpr const auto PIN_MISO = (gpio_num_t) 27;

constexpr const auto LEFT_X_CHANNEL = 6;
constexpr const auto LEFT_Y_CHANNEL = 2;
constexpr const auto RIGHT_X_CHANNEL = 1;
constexpr const auto RIGHT_Y_CHANNEL = 5;
constexpr const auto POT1_CHANNEL = 0;
constexpr const auto POT2_CHANNEL = 2;
constexpr const auto BATTERY_CHANNEL = 3;

constexpr const auto DISPLAY_I2C_ADDRESS = 0x3C;
constexpr const auto ADC_I2C_ADDRESS = 0x48;


constexpr const char* TAG = "REM";

