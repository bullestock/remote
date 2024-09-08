#pragma once

#include "mirf.h"

class ForwardAirFrame;

bool init_radio(NRF24_t& dev);

bool send_frame(NRF24_t& dev,
                ForwardAirFrame& frame);

bool data_ready(NRF24_t& dev);


