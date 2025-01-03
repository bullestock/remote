#include "display.h"
#include "defs.h"

static constexpr const int STATUS_START = 4;

Display& Display::instance()
{
    static Display the_instance;
    return the_instance;
}

Display::Display()
{
    display._address = DISPLAY_I2C_ADDRESS;
    display._flip = false;

    ssd1306_init(&display, 128, 64);
    ssd1306_contrast(&display, 0xff);
    clear();

    for (int i = 0; i < NOF_INFO_LINES; ++i)
        info_lines_dirty[i] = false;
}

void Display::clear()
{
    ssd1306_clear_screen(&display, false);
    lines.clear();
    row = 0;
}

void Display::set_status(const std::string& txt)
{
    std::lock_guard<std::mutex> g(mutex);
    status = txt;
}

void Display::set_info(int line, const std::string& txt)
{
    if (line >= NOF_INFO_LINES)
        return;
    std::lock_guard<std::mutex> g(mutex);
    if (txt == info_lines[line])
        return;
    info_lines[line] = txt;
    info_lines_dirty[line] = true;
}

void Display::set_debug_info(const ForwardAirFrame& frame)
{
    std::lock_guard<std::mutex> g(mutex);
    debug_info = frame;
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

void Display::thread_body()
{
    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        std::string new_status;
        std::string info_lines_copy[NOF_INFO_LINES];
        bool info_lines_dirty_copy[NOF_INFO_LINES];
        {
            // Copy info while we have the lock
            std::lock_guard<std::mutex> g(mutex);
            if (status != last_status)
            {
                new_status = last_status = status;
            }
            for (int i = 0; i < NOF_INFO_LINES; ++i)
            {
                if ((info_lines_dirty_copy[i] = info_lines_dirty[i]))
                    info_lines_copy[i] = info_lines[i];
                info_lines_dirty[i] = false;
            }
        }
        // Update status
        if (!new_status.empty())
        {
            ssd1306_clear_line(device(), 0, false);
            ssd1306_display_text_x3(device(), 0,
                                    new_status.c_str(), new_status.size(),
                                    false);
        }
        // Update changed info lines
        for (int i = 0; i < NOF_INFO_LINES; ++i)
        {
            if (info_lines_dirty_copy[i])
            {
                ssd1306_clear_line(device(), STATUS_START + i, false);
                ssd1306_display_text(device(), STATUS_START + i,
                                     info_lines_copy[i].c_str(), info_lines_copy[i].size(),
                                     false);
            }
        }
    }
}

void display_task(void*)
{
    Display::instance().thread_body();
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
