#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Initialize TFT object

void setup()
{
    Serial.begin(9600);

    // Initialize TFT
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE);

    // Draw the Wi-Fi symbol and header text
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("Dinossauro_Conectado", 50, 10);

    // Draw the time block
    tft.fillRect(10, 40, 300, 100, TFT_WHITE);
    tft.setTextDatum(TC_DATUM);
    tft.setTextSize(6);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("12:00:32", 160, 70);
    tft.setTextSize(2);
    tft.drawString("20 de maio de 2024", 160, 130);

    // Draw the time range
    tft.setTextSize(2);
    tft.drawString("08:00", 40, 150);
    tft.drawString("20:00", 270, 150);

    // Draw the divider line
    tft.drawLine(10, 170, 310, 170, TFT_BLACK);

    // Draw the icons and values
    tft.setTextDatum(TC_DATUM);

    // Humidity
    tft.setTextSize(3);
    tft.drawString("32%", 60, 200);
    // tft.drawCircle(40, 200, 20, TFT_BLACK); // Draw water drop icon

    // Temperature
    tft.drawString("22 C", 160, 200);
    // tft.drawBitmap(140, 180, thermometer_icon, 24, 24, TFT_BLACK); // Use your thermometer icon bitmap

    // Brightness
    tft.drawString("80%", 260, 200);
    // tft.drawBitmap(240, 180, sun_icon, 24, 24, TFT_BLACK); // Use your sun icon bitmap
}

void loop()
{
    // Nothing to do in loop for now
}
