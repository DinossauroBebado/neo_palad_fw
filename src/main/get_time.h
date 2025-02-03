#include <Rtc_Pcf8563.h>
#include "time.h"
#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "DINO_network";
const char *wifipw = "network@Dino32";
Rtc_Pcf8563 rtc;
struct tm timeinfo;
int dayOffWeek;
void setTimezone(String timezone)
{
    setenv("TZ", timezone.c_str(), 1);
    tzset();
}

// Initialize NTP
bool initTime(String timezone)
{
    configTime(-3 * 3600, 0, "pool.ntp.org", "b.st1.ntp.br");
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain NTP time");
        return false;
    }
    setTimezone(timezone);
    return true;
}

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
