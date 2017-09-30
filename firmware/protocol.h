#pragma once

#include <stdint.h>

#pragma pack(push, 1)
struct ForwardAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A5;

    uint16_t magic;
    uint16_t ticks;
    int16_t left_x;
    int16_t left_y;
    int16_t right_x;
    int16_t right_y;
    uint8_t left_pot;
    uint8_t right_pot;
    // Bits 0-3: Pushbuttons
    // Bits 8-15: Toggle switches (2 bits each)
    uint16_t switches;
    uint16_t crc;
};

struct ReturnAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A5;

    uint16_t magic;
    uint16_t ticks;
    // mV
    uint16_t battery;
    uint16_t crc;
};

#pragma pack(pop)

#include "checksum.h"

template<typename T>
void set_crc(T& frame)
{
    frame.crc = crc_16(reinterpret_cast<const unsigned char*>(&frame), sizeof(frame) - sizeof(frame.crc));
}

template<typename T>
bool check_crc(const T& frame)
{
    const auto crc = crc_16(reinterpret_cast<const unsigned char*>(&frame), sizeof(frame) - sizeof(frame.crc));
    return crc == frame.crc;
}

