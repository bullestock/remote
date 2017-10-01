#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <RF24.h>

#include "protocol.h"

const char* VERSION = "0.0.3";

byte addresses[][6] = { "1BULL", "2BULL" };

// Set up nRF24L01 radio on SPI bus (11, 12, 13) plus pins 7 & 8
RF24 radio(7, 8);

const int OLED_SDA = 5;
const int OLED_SCK = 6;
const int OLED_DC = 3;
const int OLED_CS = 2;
const int OLED_RESET = 4;
const int LEFT_X_PIN = A0;
const int LEFT_Y_PIN = A1;
const int RIGHT_X_PIN = A2;
const int RIGHT_Y_PIN = A3;
const int SHIFT_CLOCK = 9;
const int SHIFT_OUT = 10;
const int SHIFT_LOAD = A5;
const int SWITCH_PIN = A4;
const int POT1_PIN = A6;
const int POT2_PIN = A7;

Adafruit_SSD1306 display(OLED_SDA, OLED_SCK, OLED_DC, OLED_RESET, OLED_CS);

const int NOF_DELAY_SAMPLES = 100;
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
    pinMode(SWITCH_PIN, OUTPUT);
    pinMode(POT1_PIN, INPUT);
    pinMode(POT2_PIN, INPUT);
            
    Serial.begin(115200);
    Serial.println("Init");
    display.begin(SSD1306_SWITCHCAPVCC);
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Version ");
    display.print(VERSION);
    display.display();
    delay(1000);

    if (!radio.begin())
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.print("No radio!");
        display.display();
        while (1)
        {
            Serial.println("FATAL ERROR: No radio found");
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
  
    display.clearDisplay();
    display.setCursor(0, 1);
    display.setTextSize(2);
    display.print(F("Ready"));
    display.display();
    delay(100);

    for (int i = 0; i < NOF_DELAY_SAMPLES; ++i)
        delay_samples[i] = 0;
    
    // Start the radio listening for data
    radio.startListening();
}

uint16_t read_switches()
{
    uint16_t ret = 0;

    // Read toggles, pass 1
    digitalWrite(SWITCH_PIN, 0);
    digitalWrite(SHIFT_CLOCK, 1);
    digitalWrite(SHIFT_LOAD, 0);
    delayMicroseconds(5);
    digitalWrite(SHIFT_LOAD, 1);
    digitalWrite(SHIFT_CLOCK, 0);
    for (int i = 0; i < 4; ++i)
    {
        auto val = digitalRead(SHIFT_OUT);
        if (!val)
            ret |= (1 << (8+2*i));
        digitalWrite(SHIFT_CLOCK, 1);
        delayMicroseconds(5);
        digitalWrite(SHIFT_CLOCK, 0);
    }

    // Read toggles, pass 2
    digitalWrite(SWITCH_PIN, 1);
    digitalWrite(SHIFT_CLOCK, 1);
    digitalWrite(SHIFT_LOAD, 0);
    delayMicroseconds(5);
    digitalWrite(SHIFT_LOAD, 1);
    digitalWrite(SHIFT_CLOCK, 0);
    for (int i = 0; i < 4; ++i)
    {
        auto val = digitalRead(SHIFT_OUT);
        if (!val)
            ret |= (2 << (8+2*i));
        digitalWrite(SHIFT_CLOCK, 1);
        delayMicroseconds(5);
        digitalWrite(SHIFT_CLOCK, 0);
    }

    // Read pushbuttons
    for (int i = 0; i < 4; ++i)
    {
        auto val = digitalRead(SHIFT_OUT);
        if (!val)
            ret |= (1 << i);
        digitalWrite(SHIFT_CLOCK, 1);
        delayMicroseconds(5);
        digitalWrite(SHIFT_CLOCK, 0);
    }
    
    return ret;
}

unsigned long failures = 0;
unsigned long crc_errors = 0;
unsigned long successes = 0;

uint16_t battery = 0;

void loop()
{
    ForwardAirFrame frame;
    frame.magic = ForwardAirFrame::MAGIC_VALUE;
    frame.ticks = micros();
    frame.left_x = analogRead(LEFT_X_PIN);
    frame.left_y = analogRead(LEFT_Y_PIN);
    frame.right_x = analogRead(RIGHT_X_PIN);
    frame.right_y = analogRead(RIGHT_Y_PIN);
    frame.left_pot = analogRead(POT1_PIN)/4;
    frame.right_pot = analogRead(POT2_PIN)/4;
    frame.switches = read_switches();
    set_crc(frame);
    
#if 0
    char buf[80];
    sprintf(buf, "X %d Y %d X %d Y %d", frame.left_x, frame.left_y, frame.right_x, frame.right_y);
    Serial.println(buf);
    sprintf(buf, "S %04X P %02X %02X", frame.switches, frame.left_pot, frame.right_pot);
    Serial.println(buf);
#endif
    
    radio.stopListening();
    if (!radio.write(&frame, sizeof(frame)))
    {
        ++failures;
        return;
    }
    
    radio.startListening();
    const auto started_waiting_at = micros();
    bool timeout = false;
    
    while (!radio.available())
    {
        if (micros() - started_waiting_at > 200000)
        {
            // Waited longer than 200 ms
            timeout = true;
            break;
        }      
    }
        
    if (timeout)
        ++failures;
    else
    {
        ++successes;
    
        ReturnAirFrame ret_frame;
        radio.read(&ret_frame, sizeof(ret_frame));
        if ((ret_frame.magic == ReturnAirFrame::MAGIC_VALUE) &&
            check_crc(ret_frame))
        {
            const uint16_t end_time = micros();

            for (int i = actual_delay_samples-1; i > 0; --i)
                delay_samples[i] = delay_samples[i-1];
            delay_samples[0] = end_time-frame.ticks;

            battery = ret_frame.battery;
        }
        else
            ++crc_errors;
    }

    uint32_t sum = 0;
    for (int i = 0; i < actual_delay_samples; ++i)
        sum += delay_samples[i];
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    char buf2[30];
    sprintf(buf2, "%lu/%lu/%lu %d.%d V", failures, crc_errors, successes,
            battery / 1000, (battery % 1000)/100);
    display.println(buf2);
    if (actual_delay_samples < NOF_DELAY_SAMPLES)
        ++actual_delay_samples;
    else
    {
        display.setTextSize(2);
        sprintf(buf2, "RTT %lu ms", (sum/NOF_DELAY_SAMPLES+500)/1000);
        display.print(buf2);
    }
    display.display();

    delay(10);
}
