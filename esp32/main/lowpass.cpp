#include "lowpass.h"

float LowPassFilter::A = 0.5;

void LowPassFilter::set_rate(float rate)
{
    A = rate;
}

float LowPassFilter::filter(float value)
{
    previous += A * (value - previous);
    return previous;
}
