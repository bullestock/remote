#pragma once

#include "defs.h"

void init_nvs();

const uint16_t* get_stick_calibration(int stick);

void set_stick_calibration(int stick, int min_val, int max_val);

// Local Variables:
// compile-command: "cd .. && idf.py build"
// End:

