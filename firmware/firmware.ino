#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <RF24.h>

#include "protocol.h"

const char* VERSION = "0.0.1";

bool radioNumber = 0;

byte addresses[][6] = { "1Node", "2Node" };

// Used to control whether this node is sending or receiving
bool role = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);

#define OLED_SDA   5
#define OLED_SCK   6
#define OLED_DC    3
#define OLED_CS    2
#define OLED_RESET 4
const int LEFT_X_PIN = A0;
const int LEFT_Y_PIN = A1;
const int RIGHT_X_PIN = A2;
const int RIGHT_Y_PIN = A3;

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
    radio.setPALevel(RF24_PA_LOW);
  
    // Open a writing and reading pipe on each radio, with opposite addresses
    if (radioNumber)
    {
        radio.openWritingPipe(addresses[1]);
        radio.openReadingPipe(1, addresses[0]);
    }
    else
    {
        radio.openWritingPipe(addresses[0]);
        radio.openReadingPipe(1, addresses[1]);
    }
  
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

void loop()
{
    AirFrame frame;
    frame.magic = AirFrame::MAGIC_VALUE;
    frame.ticks = micros();
    frame.left_x = analogRead(LEFT_X_PIN);
    frame.left_y = analogRead(LEFT_Y_PIN);
    frame.right_x = analogRead(RIGHT_X_PIN);
    frame.right_y = analogRead(RIGHT_Y_PIN);
    char buf[80];
    sprintf(buf, "X %d Y %d X %d Y %d", frame.left_x, frame.left_y, frame.right_x, frame.right_y);
    Serial.println(buf);
    frame.switches = 0; // TODO
    
    radio.stopListening();
    if (!radio.write(&frame, sizeof(frame)))
    {
        Serial.println(F("Send failed"));
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println("");
        display.print(F("Failed"));
        display.display();
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
    {
        // Describe the results
        Serial.println(F("Failed, response timed out."));
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.println("");
        display.print(F("Timeout"));
        display.display();
    }
    else
    {
        uint16_t received_tick;
        radio.read(&received_tick, sizeof(received_tick));
        const uint16_t end_time = micros();

        for (int i = actual_delay_samples-1; i > 0; --i)
            delay_samples[i] = delay_samples[i-1];
        delay_samples[0] = end_time-frame.ticks;
        uint32_t sum = 0;
        for (int i = 0; i < actual_delay_samples; ++i)
            sum += delay_samples[i];
        display.clearDisplay();
        display.setCursor(0, 0);
        if (actual_delay_samples < NOF_DELAY_SAMPLES)
        {
            ++actual_delay_samples;

            display.setTextSize(1);
            display.println(F("Connected"));
        }
        else
        {
            display.setTextSize(1);
            display.println(F("Connected"));
            display.setTextSize(2);
            display.print(sum/NOF_DELAY_SAMPLES);
            display.print(F(" us"));
        }
        display.display();
    }

    delay(10);
}
