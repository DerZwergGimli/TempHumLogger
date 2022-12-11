#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

void drawTemperature(Adafruit_SSD1306 *display, float temperature, float humidity)
{
    display->clearDisplay();
    display->setTextColor(WHITE);
    display->setTextSize(1);

    display->setCursor(0, 0);
    display->print(WiFi.macAddress());

    display->setCursor(20, 30);
    display->setTextSize(2);
    display->printf("%.2f", temperature);
    display->setTextSize(1);
    display->setCursor(80, 30);
    display->print(" *C");

    display->setTextSize(2);
    display->setCursor(20, 50);
    display->printf("%.2f", humidity);
    display->setTextSize(1);
    display->setCursor(80, 50);
    display->print(" rH");

    display->display();
}