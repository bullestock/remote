#pragma once

#include <stdint.h>

#include <esp_log.h>

enum class Command
{
    None,
    Sound,
    Pwm,
    Battery,
};

enum class SoundCommand
{
    ListSounds,
    PlaySound,
    StopSound,
};

#pragma pack(push, 1)
struct ForwardAirFrame
{
    // Version 1 was A5A6
    // Version 2 was A5A7
    // Version 3 was A5A8
    static const uint16_t MAGIC_VALUE = 0xA5A9;

    static const int SOUND_RANDOM = 0xFFFF;
    
    uint16_t magic;             // 0
    int64_t ticks;              // 2

    // Joysticks: (-1, 1)
    float left_x = 0.0;         // 6
    float left_y = 0.0;         // 8
    float right_x = 0.0;        // 10
    float right_y = 0.0;        // 12

    Command command = Command::None;

    union
    {
        struct
        {
            SoundCommand sound_command;
            uint16_t index;
            // 0-4095
            uint16_t volume;
        } sound;
        struct
        {
            // 0-3
            uint8_t index;
            /// Duty cycle (00-FF)
            uint8_t duty;
            /// 00: 1 Hz
            /// 01: 2 Hz
            /// ...
            /// FF: 256 Hz
            uint8_t frequency;
        } pwm;
    } data;

    uint16_t crc;
};

struct ReturnAirFrame
{
    static const uint16_t MAGIC_VALUE = 0xA5A9;
    static const size_t TRACK_NAME_SIZE = 20;
    
    uint16_t magic;     // 2
    int64_t ticks;      // 8
    Command command;

    union
    {
        struct
        {
            uint16_t mV;
        } battery;
        struct
        {
            uint16_t index;
            uint16_t track_count;
            char track[TRACK_NAME_SIZE];
        } track;
    } data;
    
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
