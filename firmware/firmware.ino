#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <RF24.h>

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

Adafruit_SSD1306 display(OLED_SDA, OLED_SCK, OLED_DC, OLED_RESET, OLED_CS);

void setup()
{
    Serial.begin(115200);
    Serial.println("Init");
    display.begin(SSD1306_SWITCHCAPVCC);
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Version ");
    display.print(VERSION);
    display.display();
    delay(1000);

    if (!radio.begin())
    {
        display.clearDisplay();
        display.setCursor(0, 0);
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
    display.print(F("Ready"));
    display.display();
    delay(100);

    // Start the radio listening for data
    radio.startListening();
}

void loop()
{
    /****************** Ping Out Role ***************************/  
    if (role == 1)
    {
        radio.stopListening();                                    // First, stop listening so we can talk.
    
        Serial.println(F("Now sending"));

        unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
        if (!radio.write(&start_time, sizeof(unsigned long)))
        {
            Serial.println(F("Send failed"));
            display.clearDisplay();
            display.setCursor(0, 1);
            display.print(F("Send failed"));
            display.display();
        }
        
        radio.startListening();                                    // Now, continue listening
    
        unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
        boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
        while (!radio.available())
        {                             // While nothing is received
            if (micros() - started_waiting_at > 200000)
            {
                // If waited longer than 200ms, indicate timeout and exit while loop
                timeout = true;
                break;
            }      
        }
        
        if (timeout)
        {
            // Describe the results
            Serial.println(F("Failed, response timed out."));
            display.clearDisplay();
            display.setCursor(0, 1);
            display.print(F("Timeout"));
            display.display();
        }
        else
        {
            unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
            radio.read(&got_time, sizeof(unsigned long));
            unsigned long end_time = micros();
        
            // Spew it
            Serial.print(F("Sent "));
            Serial.print(start_time);
            Serial.print(F(", Got response "));
            Serial.print(got_time);
            Serial.print(F(", Round-trip delay "));
            Serial.print(end_time-start_time);
            Serial.println(F(" microseconds"));
            display.clearDisplay();
            display.setCursor(0, 1);
            display.print(F("Delay "));
            display.print(end_time-start_time);
            display.display();
        }

        // Try again 1s later
        delay(1000);
    }



    /****************** Pong Back Role ***************************/

    if (role == 0)
    {
        unsigned long got_time;
    
        if(radio.available())
        {
            // Variable for the received timestamp
            while (radio.available())
            {
                // While there is data ready
                radio.read(&got_time, sizeof(unsigned long));             // Get the payload
            }
     
            radio.stopListening();                                        // First, stop listening so we can talk   
            radio.write(&got_time, sizeof(unsigned long));              // Send the final one back.      
            radio.startListening();                                       // Now, resume listening so we catch the next packets.     
            Serial.print(F("Sent response "));
            Serial.println(got_time);  
        }
        else
            Serial.println("Nuffin");
    }




    /****************** Change Roles via Serial Commands ***************************/

    if (Serial.available())
    {
        char c = toupper(Serial.read());
        if (c == 'T' && role == 0)
        {      
            Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
            role = 1;                  // Become the primary transmitter (ping out)
        }
        else if (c == 'R' && role == 1)
        {
            Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
            role = 0;                // Become the primary receiver (pong back)
            radio.startListening();
            
        }
    }
    
}
