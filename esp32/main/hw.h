#pragma once

#include <stdint.h>

#include "protocol.h"

void init_hardware();

int read_adc(int channel);

void read_switches(ForwardAirFrame& frame);

