#pragma once

class ForwardAirFrame;

#include <stdint.h>

struct Switch_state
{
    enum Toggle_state
    {
        Up,
        Center,
        Down
    };
    Toggle_state toggles[4];
    uint8_t pushbuttons;
    Toggle_state slide;
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
