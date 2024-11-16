#pragma once

#include "defs.h"

void init_nvs();

void set_stick_calibration(int stick, int min_val, int max_val);

bool set_peer_mac(const char* peer_mac);

bool set_lowpass_rate(float rate);

const uint16_t* get_stick_calibration(int stick);

const char* get_peer_mac();

float get_lowpass_rate();

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:

