#include "lowpass.h"

float LowPassFilter::A = 0.5;

void LowPassFilter::set_rate(float rate)
{
    A = rate;
}

float LowPassFilter::filter(float value, bool initial)
{
    if (value >= 4096)
        return value;
    if (initial)
        previous = value;
    else
        previous += A * (value - previous);
    return previous;
}
