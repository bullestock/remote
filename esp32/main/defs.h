#pragma once

#include <driver/gpio.h>

constexpr const char* VERSION = "0.0.1";

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

constexpr const char* TAG = "REM";

