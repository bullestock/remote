#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* VERSION = "0.0.1";

#define OLED_SDA   5
#define OLED_SCK   6
#define OLED_DC    3
#define OLED_CS    2
#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_SDA, OLED_SCK, OLED_DC, OLED_RESET, OLED_CS);

void setup()
{
    Serial.begin(115200);
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
}

void loop()
{
    display.clearDisplay();
    display.setCursor(0, 1);
    display.print("Ready");
    display.display();
    delay(100);
}
