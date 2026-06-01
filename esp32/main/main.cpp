#include "console.h"
#include "defs.h"
#include "display.h"
#include "format.h"
#include "hw.h"
#include "nvs.h"
#include "radio.h"

#include <chrono>
#include <random>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

#include "protocol.h"

static std::vector<std::string> effects;
static std::vector<std::string> music;
static size_t track_count = 0;

static void handle_switches(std::default_random_engine& generator,
                            ForwardAirFrame& frame,
                            Display& display);

extern "C"
void app_main(void)
{
    printf("Remote v %s\n", VERSION);

    init_hardware();

    Display& display = Display::instance();

    init_nvs();
    
    bool debug = false;
    if (init_radio())
    {
        display.add_progress("Radio OK");
        printf("ESP-NOW init OK\n");
    }
    else
    {
        printf("ESP-NOW init failed!\n");
        display.add_progress("ERROR: No radio");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        debug = true;
    }

    if (!debug)
    {
        printf("\n\nPress a key to enter console\n");
        int keypresses = 0;
        display.add_progress("Console wait");
        for (int i = 0; i < 20; ++i)
        {
            if (getchar() != EOF)
            {
                ++keypresses;
                display.add_progress("<key>");
                if (keypresses > 3)
                {
                    debug = true;
                    break;
                }
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    if (debug)
    {
        display.add_progress("Start console");
        run_console(display);        // never returns
    }

    printf("\nStarting application\n");
    xTaskCreate(display_task, "display_task", 4*1024, NULL, 1, NULL);

    int consecutive_errors = 0;
    unsigned long failures = 0;
    unsigned long crc_errors = 0;
    unsigned long sent = 0;
    int64_t good_frames = 0;
    int64_t last_track_requested = -1;
    float their_battery = 0.0;

    // Round trip delay in microseconds
    const int NOF_DELAY_SAMPLES = 10;
    int64_t delay_samples[NOF_DELAY_SAMPLES];
    int actual_delay_samples = 0;

    display.clear();
    display.set_status("Ready");
    int count = 90;
    bool initial = true;

    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    while (1)
    {
        const auto send_time = esp_timer_get_time();
        ForwardAirFrame frame;
        if (!fill_frame(frame, send_time, initial))
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }
        initial = false;

        const auto my_battery = get_my_battery();

        handle_switches(generator, frame, display);
        
        if ((good_frames > 10) && (frame.command == Command::None))
        {
            // We have established communication. Decide which command to send.
            if (effects.empty() || (effects.size() < track_count))
            {
                if (last_track_requested >= 0 &&
                    (good_frames - last_track_requested < 10))
                {
                    //printf("Waiting\n");
                }
                else
                {
                    frame.data.sound.index = effects.size();
                    printf("Requesting track %d\n", frame.data.sound.index);
                    frame.command = Command::Sound;
                    frame.data.sound.sound_command = SoundCommand::ListEffects;
                    last_track_requested = good_frames;
                }
            }
            else if (music.empty() || (music.size() < track_count))
            {
                if (last_track_requested >= 0 &&
                    (good_frames - last_track_requested < 10))
                {
                    //printf("Waiting\n");
                }
                else
                {
                    frame.data.sound.index = music.size();
                    printf("Requesting track %d\n", frame.data.sound.index);
                    frame.command = Command::Sound;
                    frame.data.sound.sound_command = SoundCommand::ListMusic;
                    last_track_requested = good_frames;
                }
            }
        }

        if (++count > 100)
        {
            count = 0;
            display.set_debug_info(frame);
            display.set_info(0,
                             format("T %d", (int) effects.size()));
            int delay = 0;
            std::string delay_info;
            if (actual_delay_samples > 0)
            {
                int64_t sum = 0;
                for (int i = 0; i < actual_delay_samples; ++i)
                    sum += delay_samples[i];
                delay = sum/actual_delay_samples;
                delay_info = format("%d ms", delay/1000);
            }
            display.set_info(1,
                             format("S %d E %d", sent, failures));
            std::string peer_bat = "---";
            if (their_battery > 0)
                peer_bat = format("%2.2fV", their_battery);
            display.set_info(2,
                             format("%1.2fV  %s  %s",
                                    my_battery,
                                    peer_bat.c_str(),
                                    delay_info.c_str()));
        }
        
        bool ok = send_frame(frame);
        if (ok)
            consecutive_errors = 0;
        else
            ++consecutive_errors;

        bool ready = false;
        if (ok)
        {
            ++sent;
            if (sent >= 10000)
            {
                sent = 0;
                failures = 0;
            }

            ReturnAirFrame ret_frame;
            for (int i = 0; !ready && (i < 10); ++i)
            {
                if (xSemaphoreTake(receive_mutex, portMAX_DELAY) == pdTRUE)
                {
                    if (data_ready)
                    {
                        data_ready = false;
                        memcpy(&ret_frame, &received_frame, sizeof(ReturnAirFrame));
                        ready = true;
                    }
                    xSemaphoreGive(receive_mutex);
                }
                vTaskDelay(1);
            }
            if (!ready)
            {
                if (ready)
                    printf("Not ready\n");
                ++failures;
            }
            else
            {
                display.set_status("Ready");
                if (ret_frame.magic != ReturnAirFrame::MAGIC_VALUE)
                {
                    printf("Bad magic: %04X\n", ret_frame.magic);
                    ++failures;
                }
                else if (!check_crc(ret_frame))
                {
                    printf("Bad CRC\n");
                    ++crc_errors;
                }
                else
                {
                    const auto end_time = esp_timer_get_time();
                    ++good_frames;

                    for (int i = actual_delay_samples-1; i > 0; --i)
                        delay_samples[i] = delay_samples[i-1];
                    delay_samples[0] = end_time - send_time;
                    if (actual_delay_samples < NOF_DELAY_SAMPLES)
                        ++actual_delay_samples;
                    switch (ret_frame.command)
                    {
                    case Command::None:
                    case Command::Pwm:
                        break;

                    case Command::Battery:
                        their_battery = ret_frame.data.battery.mV/1000.0;
                        break;

                    case Command::Sound:
                        {
                            track_count = ret_frame.data.track.track_count;
                            printf("Received track %u of %u: %s\n",
                                   ret_frame.data.track.index, track_count, ret_frame.data.track.track);
                            auto& v = ret_frame.data.track.command == SoundCommand::ListEffects ? effects : music;
                            if (ret_frame.data.track.index == 0)
                                v.clear();
                            if (ret_frame.data.track.index < v.size())
                                printf("SKIP %d\n", ret_frame.data.track.index);
                            else
                                v.push_back(ret_frame.data.track.track);
                        }
                        break;
                    }
                }
            }
        }
        if (consecutive_errors > 10)
            display.set_status("!!!!!!!!!!!!!!!!");
        else if (!ready)
            display.set_status("\x81\x81\x81\x81\x81\x81\x81\x81"
                               "\x81\x81\x81\x81\x81\x81\x81\x81");
        else
            display.set_status("\x80\x80\x80\x80\x80\x80\x80\x80"
                               "\x80\x80\x80\x80\x80\x80\x80\x80");
    }
}

static void handle_switches(std::default_random_engine& generator,
                            ForwardAirFrame& frame,
                            Display& display)
{
    static Switch_state last_switch_state;
    static bool first = true;
    static int random_sound = -1;
    static int fixed_sound = 0;

    if (first)
    {
        memset(&last_switch_state, 0, sizeof(last_switch_state));
        first = false;
    }
    const auto switch_state = read_switches();
    if (!memcmp(&switch_state, &last_switch_state, sizeof(last_switch_state)))
        return;
    last_switch_state = switch_state;
    // Effects mode
    switch (switch_state.toggles[0])
    {
    case Switch_state::Up:
        // Random
        if (random_sound < 0)
        {
            std::uniform_int_distribution<int> distribution(0, effects.size());
            random_sound = distribution(generator);
            printf("New random sound: %d\n", random_sound);
            display.set_info(3, format("R %s", effects[random_sound].c_str()));
        }
        break;
    case Switch_state::Center:
        // Browse
    case Switch_state::Down:
        // Fixed
        break;
    }
    // Play sound?
    if (switch_state.pushbuttons & 0x20)
    {
        switch (switch_state.toggles[0])
        {
        case Switch_state::Up:
            // Random
            frame.command = Command::Sound;
            frame.data.sound.sound_command = SoundCommand::PlaySound;
            frame.data.sound.index = random_sound;
            printf("Playing random sound (%d)\n", random_sound);
            random_sound = -1;
            break;
        case Switch_state::Center:
            // Browse
            break;
        case Switch_state::Down:
            // Fixed
            frame.command = Command::Sound;
            frame.data.sound.sound_command = SoundCommand::PlaySound;
            frame.data.sound.index = fixed_sound;
            printf("Playing fixed sound (%d)\n", fixed_sound);
            break;
        }
    }
}

// Local Variables:
// compile-command: "(cd ..; idf.py build)"
// End:
