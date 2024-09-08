#include "display.h"
#include "defs.h"

static constexpr const int STATUS_START = 4;

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
    lines.clear();
    row = 0;
}

void Display::set_status(const std::string& txt)
{
    if (!last_status.empty() && txt == last_status)
        return;
    ssd1306_clear_line(device(), 0, false);
    ssd1306_display_text_x3(device(), 0, txt.c_str(), txt.size(), false);
    last_status = txt;
}

void Display::set_info(int line, const std::string& txt)
{
    ssd1306_clear_line(device(), STATUS_START + line, false);
    ssd1306_display_text(device(), STATUS_START + line, txt.c_str(), txt.size(), false);
}

SSD1306_t* Display::device()
{
    return &display;
}

void Display::add_progress(const std::string& status)
{
    std::string txt = std::string(" ") + status;
    ssd1306_display_text(device(), row, txt.c_str(), txt.size(), false);
    ++row;
    lines.push_back(status);
    if (row < 7)
        return; // still room for more
    // Out of room, scroll up
    lines.erase(lines.begin());
    --row;
    clear();
    for (int i = 0; i < lines.size(); ++i)
    {
        ssd1306_display_text(device(), i, lines[i].c_str(), lines[i].size(), false);
    }
}
