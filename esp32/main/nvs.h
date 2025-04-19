#pragma once

#include "defs.h"

void init_nvs();

struct calibration_data
{
    uint16_t min;
    uint16_t mid;
    uint16_t max;
};

void set_stick_calibration(int stick,
                           int min_val,
                           int mid_val,
                           int max_val);

bool set_peer_mac(const char* peer_mac);

bool set_lowpass_rate(float rate);

const calibration_data& get_stick_calibration(int stick);

const char* get_peer_mac();

float get_lowpass_rate();

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:

