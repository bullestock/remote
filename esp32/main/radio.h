#pragma once

#include "protocol.h"

#include <stdint.h>

#include "mirf.h"

bool init_radio(NRF24_t& dev);

bool send_frame(NRF24_t& dev,
                ForwardAirFrame& frame,
                bool& timeout);

