#pragma once

class ForwardAirFrame;

#include <stdint.h>

void init_hardware();

int read_adc(int channel);

void read_switches(ForwardAirFrame& frame);

bool fill_frame(ForwardAirFrame& frame,
                int64_t ticks,
                bool initial);

/// Get averaged battery voltage in V
float get_my_battery();
