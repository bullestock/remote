#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "protocol.h"
#include "ssd1306.h"

class Display
{
public:
    static Display& instance();

    void clear();

    SSD1306_t* device();

    /// Add progress message (used during boot).
    void add_progress(const std::string& status);

    /// Set status.
    void set_status(const std::string& txt);

    /// Set an info line.
    void set_info(int line, const std::string& txt);

    /// Set debug info.
    void set_debug_info(const ForwardAirFrame& frame);

private:
    Display();

    void thread_body();
    
    SSD1306_t display;
    bool display_present = false;
    // Used by add_progress()
    int row = 0;
    std::vector<std::string> lines;

    static const int NOF_INFO_LINES = 3;
    std::mutex mutex;
    std::string status;
    std::string last_status;
    ForwardAirFrame debug_info;
    ForwardAirFrame last_debug_info;
    std::string info_lines[NOF_INFO_LINES];
    bool info_lines_dirty[NOF_INFO_LINES];

    friend void display_task(void*);
};

void display_task(void*);

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
