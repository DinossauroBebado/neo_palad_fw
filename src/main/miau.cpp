#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <WiFi.h>
#include "time.h"
#include <TFT_eSPI.h>
#include <SPI.h>

// Pin Definitions
#define light_pin 33
#define screen_pin 32
#define ldr_pin 36
#define dht11_pin 14

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN dht11_pin // Digital pin connected to the DHT sensor
// Feather HUZZAH 4ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE    DHT22     // DHT 22 (AM2302)
// #define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

TFT_eSPI tft = TFT_eSPI(); // Initialize TFT object

// TFT and Sprite Initialization

// Wi-Fi Configuration
const char *ssid = "DINO_network";
const char *wifipw = "network@Dino32";

// const char *ssid = "rededoprojeto";
// const char *wifipw = "arededoprojeto";

// RTC and NTP Variables
Rtc_Pcf8563 rtc;
bool rtcInitialized = false;
struct tm timeinfo;
unsigned long lastUpdate = 0;
unsigned long updateInterval = 1000; // 1-second update interval

// Timezone Function
void setTimezone(String timezone)
{
  setenv("TZ", timezone.c_str(), 1);
  tzset();
}

// Initialize NTP
bool initTime(String timezone)
{
  configTime(0, 0, "a.st1.ntp.br", "b.st1.ntp.br");
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain NTP time");
    return false;
  }
  setTimezone(timezone);
  return true;
}

// Set RTC Time from NTP
void setRtcTimeFromNtp()
{
  rtc.setDate(timeinfo.tm_mday, timeinfo.tm_wday, timeinfo.tm_mon + 1, false, timeinfo.tm_year - 100);
  rtc.setTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

// Start Wi-Fi
void startWifi()
{
  WiFi.begin(ssid, wifipw);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000)
  {
    delay(500);
  }
}

// Wi-Fi Reconnect Function
void reconnectWifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    startWifi();
  }
}

// Setup Function
void setup()
{
  Serial.begin(9600);

  dht.begin();
  sensor_t sensor;

  pinMode(light_pin, OUTPUT);
  pinMode(screen_pin, OUTPUT);
  pinMode(ldr_pin, INPUT);

  startWifi();
  initTime("<GMT-3>");
  rtc.initClock();

  if (WiFi.status() == WL_CONNECTED)
  {
    setRtcTimeFromNtp();
  }

  // Initialize TFT
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);

  delayMS = sensor.min_delay / 1000;
}

// Loop Function
void loop()
{
  // delay(delayMS);
  int ldr_read = analogRead(ldr_pin);
  ldr_read = map(ldr_read, 0, 4096, 0, 100);
  Serial.println(ldr_read);

  sensors_event_t event;

  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= updateInterval)
  {
    lastUpdate = currentMillis;

    // Control light and screen based on time
    if (rtc.getHour() >= 8 && rtc.getHour() <= 20)
    {
      digitalWrite(light_pin, HIGH);
      digitalWrite(screen_pin, HIGH);
    }
    else
    {
      digitalWrite(light_pin, LOW);
      digitalWrite(screen_pin, LOW);
    }
  }

  reconnectWifi();

  // Draw the Wi-Fi symbol and header text
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString(ssid, 50, 10);

  // Draw the time block
  tft.fillRect(10, 40, 300, 100, TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(6);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(rtc.formatTime(), 160, 70);
  tft.setTextSize(2);
  tft.drawString(rtc.formatDate(), 160, 130);

  // Draw the time range
  tft.setTextSize(2);
  tft.drawString("08:00", 40, 150);
  tft.drawString("20:00", 270, 150);

  // Draw the divider line
  tft.drawLine(10, 170, 310, 170, TFT_BLACK);

  // Draw the icons and values
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(3);

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    String humidity_str = String(event.relative_humidity, 1); // Convert float to String with 1 decimal place
    tft.drawString(humidity_str + " %", 60, 200);
  }
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    // Temperature
    String temp_str = String(event.temperature, 1); // Convert float to String with 1 decimal place
    tft.drawString(temp_str + " C", 170, 200);
  }

  // Humidity
  // Append "%" and display
  // tft.drawCircle(40, 200, 20, TFT_BLACK); // Draw water drop icon

  // Brightness
  String brightness_str = String(ldr_read); // Convert float to String with 1 decimal place
  tft.drawString(brightness_str + "%", 260, 200);
  // tft.drawBitmap(240, 180, sun_icon, 24, 24, TFT_BLACK); // Use your sun icon bitmap

  // tft.fillScreen(TFT_WHITE); // Limpa a tela inteira
  delay(100);
}