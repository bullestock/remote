#pragma once

class ForwardAirFrame;

#include <stdint.h>

struct Switch_state
{
    /// Two bits per switch:
    /// 01  Up
    /// 00  Center
    /// 10  Down
    uint8_t toggles;
    uint8_t pushbuttons;
    /// 01  Top
    /// 00  Center
    /// 10  Bottom
    uint8_t slide;
};

void init_hardware();

int read_adc(int channel);

float read_stick(int stick, bool initial);

Switch_state read_switches();

bool fill_frame(ForwardAirFrame& frame,
                int64_t ticks,
                bool initial);

/// Get averaged battery voltage in V
float get_my_battery();
