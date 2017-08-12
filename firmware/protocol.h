#pragma once

#pragma pack(push, 1)
struct AirFrame
{
    static const int MAGIC_VALUE = 0xA5A5;

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
};

#pragma pack(pop)

