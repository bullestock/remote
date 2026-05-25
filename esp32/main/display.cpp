#include "display.h"
#include "defs.h"
#include "format.h"

#include <string.h>

#include <freertos/FreeRTOS.h>

static constexpr const int STATUS_START = 2;

Display& Display::instance()
{
    static Display the_instance;
    return the_instance;
}

Display::Display()
{
    display._address = DISPLAY_I2C_ADDRESS;
    display._x_offset = 96;

    display_present = sh1107_init(&display, 128, 128);
    if (!display_present)
    {
        ESP_LOGE(TAG, "Display not present");
        return;
    }

    sh1107_contrast(&display, 0xff);
    clear();

    for (int i = 0; i < NOF_INFO_LINES; ++i)
        info_lines_dirty[i] = false;

    memset(&debug_info, 0, sizeof(debug_info));
    memset(&last_debug_info, 0, sizeof(last_debug_info));
    last_debug_info.right_x = 1000;
}

void Display::clear()
{
    if (!display_present)
        return;
    sh1107_clear_screen(&display, false);
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

SH1107_t* Display::device()
{
    return &display;
}

void Display::add_progress(const std::string& status)
{
    if (!display_present)
        return;
    sh1107_display_text(device(), row, 0, status.c_str(), status.size(), false);
    ++row;
    if (row >= 16)
        row = 0;
}

void Display::thread_body()
{
    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if (!display_present)
            continue;
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
            sh1107_clear_line(device(), 0, false);
            sh1107_display_text(device(), 0, 0,
                                new_status.c_str(), new_status.size(),
                                false);
        }
        // Update changed info lines
        for (int i = 0; i < NOF_INFO_LINES; ++i)
        {
            if (info_lines_dirty_copy[i])
            {
                sh1107_clear_line(device(), STATUS_START + i, false);
                sh1107_display_text(device(), STATUS_START + i, 0,
                                    info_lines_copy[i].c_str(),
                                    info_lines_copy[i].size(),
                                    false);
            }
        }
        // Update debug
        if (debug_info.right_x != last_debug_info.right_x ||
            debug_info.right_y != last_debug_info.right_y)
        {
            const auto txt = format("R %.2f %.2f",
                                    debug_info.right_x, debug_info.right_y);
            sh1107_clear_line(device(), STATUS_START + NOF_INFO_LINES, false);
            sh1107_display_text(device(),
                                STATUS_START + NOF_INFO_LINES, 0,
                                txt.c_str(), txt.size(),
                                false);
            last_debug_info = debug_info;
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
