#include <SPI.h>
#include <Wire.h>
#include <RF24.h>

#include "SH1106.h"

#include "protocol.h"

//#define NO_RADIO

const char* const VERSION PROGMEM = "0.0.4";

const byte addresses[][6] = { "1BULL", "2BULL" };

#ifndef NO_RADIO
// Set up nRF24L01 radio on SPI bus (11, 12, 13) plus pins 7 & 8
RF24 radio(7, 8);
#endif

const int LEFT_X_PIN = A0;
const int LEFT_Y_PIN = A1;
const int RIGHT_X_PIN = A3;
const int RIGHT_Y_PIN = A2;
const int SHIFT_CLOCK = 9;
const int SHIFT_OUT = 10;
const int SHIFT_LOAD = 2;
const int AUX1_PIN = 3;
const int AUX2_PIN = 4;
const int AUX3_PIN = 5;
const int AUX4_PIN = 6;
const int POT1_PIN = A6;
const int POT2_PIN = A7;

Adafruit_SH1106 display;

const int NOF_DELAY_SAMPLES = 10;
uint16_t delay_samples[NOF_DELAY_SAMPLES];
int actual_delay_samples = 0;

void setup()
{
    pinMode(LEFT_X_PIN, INPUT);
    pinMode(LEFT_Y_PIN, INPUT);
    pinMode(RIGHT_X_PIN, INPUT);
    pinMode(RIGHT_Y_PIN, INPUT);
    pinMode(SHIFT_CLOCK, OUTPUT);
    pinMode(SHIFT_OUT, INPUT);
    pinMode(SHIFT_LOAD, OUTPUT);
    pinMode(AUX1_PIN, INPUT);
    pinMode(AUX2_PIN, INPUT);
    pinMode(AUX3_PIN, INPUT);
    pinMode(AUX4_PIN, INPUT);
    pinMode(POT1_PIN, INPUT);
    pinMode(POT2_PIN, INPUT);

    Serial.begin(115200);

    if (!display.begin(SH1106_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("No disp");
        while (1)
            ;
    }
    //delay(1000);
    display.setpage(1);
    display.clearDisplay();
    display.display();
    display.setpage(0);
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Version ");
    display.print(VERSION);
    display.display();
    delay(1000);

#ifndef NO_RADIO
    if (!radio.begin())
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("No radio!");
        display.display();
        while (1)
        {
            //Serial.println("FATAL ERROR: No radio found");
            delay(1000);
        }
    }

    // Go above WiFi channels
    radio.setChannel(108);
    // Set the PA Level low to prevent power supply related issues
    radio.setPALevel(RF24_PA_MAX);
  
    // Open a writing and reading pipe on each radio, with opposite addresses
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
#endif  
    display.clearDisplay();
    display.setCursor(0, 1);
    display.setTextSize(2);
    display.print(F("Ready"));
    display.display();
    delay(100);

    for (int i = 0; i < NOF_DELAY_SAMPLES; ++i)
        delay_samples[i] = 0;
    
#ifndef NO_RADIO
    // Start the radio listening for data
    radio.startListening();
#endif
}

void read_switches(ForwardAirFrame& frame)
{
    uint16_t tmp = 0;

    digitalWrite(SHIFT_CLOCK, 1);
    digitalWrite(SHIFT_LOAD, 0);
    delayMicroseconds(5);
    digitalWrite(SHIFT_LOAD, 1);
    digitalWrite(SHIFT_CLOCK, 0);
    for (int i = 0; i < 16; ++i)
    {
        auto val = digitalRead(SHIFT_OUT);
        //Serial.print(i); Serial.print(": "); Serial.println(val);
        if (!val)
            tmp |= (1 << i);
        digitalWrite(SHIFT_CLOCK, 1);
        delayMicroseconds(5);
        digitalWrite(SHIFT_CLOCK, 0);
    }

    // tmp now contains:
    //
    // 1111110000000000
    // 5432109876543210
    // T3T4T1T2PPPPPPS1
    //         456123

    // Swap P4 with P6 and P1 with P3
    uint8_t pushbuttons = (tmp & 0xFC) >> 2;
    frame.pushbuttons = (pushbuttons & 0x12)
       + (pushbuttons & 0x20 ? 0x08 : 0)
       + (pushbuttons & 0x08 ? 0x20 : 0)
       + (pushbuttons & 0x04 ? 0x01 : 0)
       + (pushbuttons & 0x01 ? 0x04 : 0);

    // Swap T3 with T4 and T1 with T2
    uint16_t toggles = (tmp & 0xFF00) >> 8;
    frame.toggles =
       ((toggles & 0xC0) >> 2) +
       ((toggles & 0x30) << 2) +
       ((toggles & 0x0C) >> 2) +
       ((toggles & 0x03) << 2);
    
    frame.slide = tmp & 0x03;
}

unsigned long failures = 0;
unsigned long crc_errors = 0;
unsigned long successes = 0;

uint16_t battery = 0;

bool blink_state = false;
int blink_count = 0;

bool send_frame(unsigned long ticks,
                ForwardAirFrame& frame,
                bool& timeout)
{
    frame.magic = ForwardAirFrame::MAGIC_VALUE;
    frame.ticks = ticks;
    frame.left_x = 1023 - analogRead(LEFT_X_PIN);
    frame.left_y = analogRead(LEFT_Y_PIN);
    frame.right_x = 1023 - analogRead(RIGHT_X_PIN);
    frame.right_y = analogRead(RIGHT_Y_PIN);
    frame.left_pot = analogRead(POT1_PIN)/4;
    frame.right_pot = analogRead(POT2_PIN)/4;
    read_switches(frame);
    set_crc(frame);
    
#if 1
    char buf[80];
    sprintf(buf, "X %3d Y %3d X %3d Y %3d S %02X %02X %d P %02X %02X",
            frame.left_x, frame.left_y, frame.right_x, frame.right_y,
            frame.pushbuttons,
            frame.toggles,
            frame.slide,
            frame.left_pot, frame.right_pot);
    Serial.println(buf);
#endif
    
#ifndef NO_RADIO
    radio.stopListening();
    if (!radio.write(&frame, sizeof(frame)))
    {
        //Serial.println("Radio write failed");
        return false;
    }
    
    radio.startListening();
    const auto started_waiting_at = micros();
    
    while (!radio.available())
        if (micros() - started_waiting_at > 200000)
        {
            // Waited longer than 200 ms
            timeout = true;
            return false;
        }
#endif
    
    return true;
}

int consecutive_errors = 0;
bool show_error = false;

void loop()
{
    const auto ticks = micros();
    bool timeout = false;
    ForwardAirFrame frame;
    bool ok = send_frame(ticks, frame, timeout);
    if (ok)
        consecutive_errors = 0;
    else
        ++consecutive_errors;

    if (timeout)
        ++failures;

    bool show_error = (consecutive_errors > 10);

    if (ok)
    {
        ++successes;
    
        ReturnAirFrame ret_frame;
#ifndef NO_RADIO
        radio.read(&ret_frame, sizeof(ret_frame));
        if ((ret_frame.magic == ReturnAirFrame::MAGIC_VALUE) &&
            check_crc(ret_frame))
        {
            const uint16_t end_time = micros();

            for (int i = actual_delay_samples-1; i > 0; --i)
                delay_samples[i] = delay_samples[i-1];
            delay_samples[0] = end_time-ticks;

            battery = ret_frame.battery;
        }
        else
        {
            ++crc_errors;
            show_error = true;
        }
#endif
    }

    uint32_t sum = 0;
    for (int i = 0; i < actual_delay_samples; ++i)
        sum += delay_samples[i];
    display.setpage(0);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    char buf2[30];
    sprintf(buf2, "%lu/%lu/%lu %d.%d V", failures, crc_errors, successes,
            battery / 1000, (battery % 1000)/100);
    if (actual_delay_samples < NOF_DELAY_SAMPLES)
        ++actual_delay_samples;
    else if (!show_error || blink_state)
        sprintf(buf2 + strlen(buf2), " %lu ms", (sum/NOF_DELAY_SAMPLES+500)/1000);

    display.println(buf2);
    display.display();

    display.setpage(1);
    display.clearDisplay();
    display.setTextSize(2);
    //display.setCursor(0, 0);

    switch (frame.slide)
    {
    case 2:
        display.print("Drive");
        break;
    case 0:
        display.print("Arm/Drive");
        break;
    case 1:
        display.print("Arm");
        break;
    default:
        display.print("Inconceivable!");
        break;
    }
    display.display();

    delay(10);
    if (++blink_count > 10)
    {
        blink_count = 0;
        blink_state = !blink_state;
    }
}
