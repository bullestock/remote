#pragma once

#include <stdint.h>

#include <esp_log.h>

#pragma pack(push, 1)
struct ForwardAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A6;

    uint16_t magic;             // 0
    int64_t ticks;              // 2
    int16_t left_x;             // 6
    int16_t left_y;             // 8
    int16_t right_x;            // 10
    int16_t right_y;            // 12
    uint16_t left_pot;          // 14
    uint16_t right_pot;         // 16
    /// Two bits per switch:
    /// 01  Up
    /// 00  Center
    /// 10  Down
    uint16_t toggles;           // 17
    uint8_t pushbuttons;        // 18
    /// 01  Top
    /// 00  Center
    /// 10  Bottom
    uint8_t slide;              // 19
    uint16_t crc;               // 20
};

struct ReturnAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A6;

    uint16_t magic;     // 2
    int64_t ticks;      // 8
    // mV
    uint16_t battery;   // 2
    uint16_t crc;       // 2
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
    const size_t len = sizeof(frame) - sizeof(frame.crc);
    const auto crc = crc_16(reinterpret_cast<const unsigned char*>(&frame), len);
    return crc == frame.crc;
}

