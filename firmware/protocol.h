#pragma once

#include <stdint.h>

#pragma pack(push, 1)
struct ForwardAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A6;

    uint16_t magic;
    uint16_t ticks;
    int16_t left_x;
    int16_t left_y;
    int16_t right_x;
    int16_t right_y;
    uint8_t left_pot;
    uint8_t right_pot;
    uint8_t pushbuttons;
    /// Two bits per switch:
    /// 01  Up
    /// 00  Center
    /// 10  Down
    uint16_t toggles;
    /// 01  Top
    /// 00  Center
    /// 10  Bottom
    uint8_t slide;
    uint16_t crc;
};

struct ReturnAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A6;

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

