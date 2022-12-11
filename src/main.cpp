

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <WiFiManager.h>

#include "sensor/sensor.h"
#include "display/display.h"
#include <InfluxDbClient.h>
#include "secrets.h"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

#define DHTPIN 25     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

uint32_t delayMS;

Point inlfux_sensor("ClimateSensors");

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

  delay(2000); // Pause for 2 seconds

  // Connect WiFi

  client.setInsecure(true);

  inlfux_sensor.addTag("device", DEVICE);
  inlfux_sensor.addTag("mac", WiFi.macAddress());
  inlfux_sensor.addTag("SSID", WiFi.SSID());

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check db-server connection
  if (client.validateConnection())
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  }
  else
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop()
{
  float value_temp, value_hum;
  getSensorEvents(dht, delayMS, value_temp, value_hum);
  drawTemperature(&display, value_temp, value_hum);

  inlfux_sensor.clearFields();
  inlfux_sensor.addField("rssi", WiFi.RSSI());
  inlfux_sensor.addField("temperature", value_temp);
  inlfux_sensor.addField("humidity", value_hum);

  Serial.print("Writing: ");
  Serial.println(inlfux_sensor.toLineProtocol());
  if (!client.writePoint(inlfux_sensor))
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  Serial.println("Wait 5min");
  delay(1000 * 60 * 5);
}