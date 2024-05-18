#include "display.h"
#include "defs.h"

Display::Display(SSD1306_t& d)
    : display(d)
{
    display._address = DISPLAY_I2C_ADDRESS;
    display._flip = false;

    ssd1306_init(&display, 128, 64);
    ssd1306_contrast(&display, 0xff);
    clear();
}

void Display::clear()
{
    ssd1306_clear_screen(&display, false);
}

SSD1306_t* Display::device()
{
    return &display;
}

