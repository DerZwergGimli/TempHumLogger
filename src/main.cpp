/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <WiFiManager.h>

#include "sensor/sensor.h"
#include "display/display.h"

#define DHTPIN 25     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

uint32_t delayMS;

void setup()
{
  Serial.begin(115200);

  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print("Booting...");
  display.display();

  delay(2000); // Pause for 2 seconds

  dht.begin();
  delayMS = init_sensor(dht);

  display.clearDisplay();
  display.print("Init...");
  display.display();

  WiFiManager wm;
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("TempMeter"); // password protected ap
  display.setTextSize(1);
  display.clearDisplay();
  display.print("Wifi-Setup...");
  display.display();

  if (!res)
  {
    Serial.println("Failed to connect");
    display.clearDisplay();
    display.print("Wifi-Failed...");
    display.display();
    ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    display.clearDisplay();
    display.print("Wifi-Connected!");
    display.display();
  }
}

void loop()
{
  float value_temp, value_hum;
  getSensorEvents(dht, delayMS, value_temp, value_hum);
  drawTemperature(&display, value_temp, value_hum);

  // // Delay between measurements.
  // delay(delayMS);
  // // Get temperature event and print its value.
  // sensors_event_t event;
  // dht.temperature().getEvent(&event);
  // if (isnan(event.temperature))
  // {
  //   Serial.println(F("Error reading temperature!"));
  // }
  // else
  // {
  //   Serial.print(F("Temperature: "));
  //   Serial.print(event.temperature);
  //   Serial.println(F("°C"));
  // }
  // // Get humidity event and print its value.
  // dht.humidity().getEvent(&event);
  // if (isnan(event.relative_humidity))
  // {
  //   Serial.println(F("Error reading humidity!"));
  // }
  // else
  // {
  //   Serial.print(F("Humidity: "));
  //   Serial.print(event.relative_humidity);
  //   Serial.println(F("%"));
  // }
}