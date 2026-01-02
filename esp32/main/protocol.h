#pragma once

#include <stdint.h>

#include <esp_log.h>

#pragma pack(push, 1)
struct ForwardAirFrame
{
    // Version 1 was A5A6
    // Version 2 was A5A7
    static const uint16_t MAGIC_VALUE = 0xA5A8;

    uint16_t magic;             // 0
    int64_t ticks;              // 2

    // Joysticks: (-1, 1)
    float left_x;             // 6
    float left_y;             // 8
    float right_x;            // 10
    float right_y;            // 12

    float volume;
    float analog;

    static const int SOUND_RANDOM = 0xFFFE;
    static const int SOUND_ABORT = 0xFFFF;

    /// 0000:      No sound
    /// 0001-FFFD: Start playing specified sound
    /// FFFE:      Random sound
    /// FFFF:      Abort sound
    uint16_t sound;

    struct
    {
        /// Duty cycle (00-FF)
        uint8_t duty;
        /// 00: 1 Hz
        /// 01: 2 Hz
        /// ...
        /// FF: 256 Hz
        uint8_t frequency;
    } pwm[4];    

    uint16_t crc;
};

struct ReturnAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A8;

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

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
