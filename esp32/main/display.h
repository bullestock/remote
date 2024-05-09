#pragma once

#include <string>
#include <vector>

#include "ssd1306.h"

class Display
{
public:
    Display(SSD1306_t& d);

    void clear();

    SSD1306_t* device();

private:
    SSD1306_t& display;
};
