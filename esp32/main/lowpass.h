#pragma once

class LowPassFilter
{
public:
    LowPassFilter()
    {
    }

    static void set_rate(float rate);

    float filter(float value);

private:
    static float A;
    float previous = 0;
};
