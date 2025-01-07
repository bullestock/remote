#pragma once

class LowPassFilter
{
public:
    LowPassFilter()
    {
    }

    static void set_rate(float rate);

    float filter(float value, bool initial = false);

private:
    static float A;
    float previous = 0;
};
