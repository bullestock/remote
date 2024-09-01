#pragma once

#include <stdint.h>

#include "protocol.h"

void init_hardware();

int read_adc(int channel);

void read_switches(ForwardAirFrame& frame);

void fill_frame(ForwardAirFrame& frame,
                int64_t ticks);

/// Get averaged battery voltage in V
float get_my_battery();
