#include <Wire.h>

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "get_time.h"

// Definições de pinos
#define light_pin 33
#define screen_pin 32
#define ldr_pin 36
#define dht11_pin 14

#define DHTPIN dht11_pin
#define DHTTYPE DHT11

// Inicialização do display e sensores
TFT_eSPI tft = TFT_eSPI();

DHT dht(DHTPIN, DHTTYPE);

// Criando sprites para diferentes elementos da tela
TFT_eSprite clockSprite = TFT_eSprite(&tft);
TFT_eSprite tempSprite = TFT_eSprite(&tft);
TFT_eSprite humiditySprite = TFT_eSprite(&tft);
TFT_eSprite dateSprite = TFT_eSprite(&tft);

// Variáveis para armazenar valores anteriores (evita atualização desnecessária)
String last_time = "";
String last_date = "";
String last_temp = "";
String last_humidity = "";

// variaveis controle da luz
bool lightOn = false;
int timeOnWeekday = 8;
int timeOffWeekday = 20;
int timeOnWeekend = 10;
int timeOffWeekend = 22;
int timeOn, timeoff;

// Função para desenhar o relógio no sprite
void drawClock(String time)
{
  clockSprite.fillSprite(TFT_BLACK);
  clockSprite.setTextColor(TFT_WHITE);
  clockSprite.setTextSize(6);
  clockSprite.drawString(time, 20, 30);
  clockSprite.pushSprite(0, 40); // Posiciona o sprite na tela
}

// Função para desenhar a temperatura no sprite
void drawTemperature(String temp)
{
  tempSprite.fillSprite(TFT_BLACK);
  tempSprite.setTextColor(TFT_RED);
  tempSprite.setTextSize(4);
  tempSprite.drawString(temp, 10, 10);
  tempSprite.pushSprite(165, 200); // Posiciona o sprite na tela
}
void drawDate(String date)
{
  dateSprite.fillSprite(TFT_BLACK);
  dateSprite.setTextColor(TFT_WHITE);
  dateSprite.setTextSize(3);
  dateSprite.drawString(date, 70, 30);
  dateSprite.pushSprite(0, 110); // Posiciona o sprite na tela
}

// Função para desenhar a umidade no sprite
void drawHumidity(String humidity)
{
  humiditySprite.fillSprite(TFT_BLACK);
  humiditySprite.setTextColor(TFT_BLUE);
  humiditySprite.setTextSize(4);
  humiditySprite.drawString(humidity, 10, 10);
  humiditySprite.pushSprite(5, 200); // Posiciona o sprite na tela
}

void setup()
{
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(4);
  tft.drawString("Init Wifi", 40, 150);

  startWifi();
  initTime("<GMT-3>");
  rtc.initClock();

  if (WiFi.status() == WL_CONNECTED)
  {
    setRtcTimeFromNtp();
  }

  Serial.begin(115200);
  tft.init();

  // rtc.begin();
  dht.begin();

  // Configurando os sprites
  clockSprite.createSprite(370, 100); // Tamanho do sprite para o relógio
  dateSprite.createSprite(370, 50);
  tempSprite.createSprite(150, 40);     // Tamanho do sprite para a temperatura
  humiditySprite.createSprite(150, 40); // Tamanho do sprite para a umidade

  tft.drawLine(10, 170, 310, 170, TFT_WHITE);
  String current_date = rtc.formatDate();
  drawDate(current_date);
  tft.fillScreen(TFT_BLACK);
}

void loop()
{
  // Atualiza hora e data
  String current_time = rtc.formatTime();
  String current_date = rtc.formatDate();
  // int weekday = rtc.getWeekday();
  int weekday = 0;

  Serial.println(weekday);
  // weekend
  if (weekday == 0 || weekday == 6)
  {
    timeOn = timeOnWeekend;
    timeoff = timeOffWeekend;
  }
  else
  {
    timeOn = timeOnWeekday;
    timeoff = timeOffWeekday;
  }

  // Atualiza temperatura e umidade
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  String temp_str = String(temp, 1) + " C";
  String humidity_str = String(humidity, 1) + " %";

  // Control light and screen based on time
  lightOn = (rtc.getHour() >= timeOn && rtc.getHour() <= timeoff);

  // Atualiza apenas se houver mudança

  if (current_date != last_date)
  {
    drawDate(current_date);
    last_date = current_date;
  }

  if (current_time != last_time)
  {
    drawClock(current_time);
    last_time = current_time;
  }

  if (temp_str != last_temp)
  {
    drawTemperature(temp_str);
    last_temp = temp_str;
  }

  if (humidity_str != last_humidity)
  {
    drawHumidity(humidity_str);
    last_humidity = humidity_str;
  }

  // controls reles

  if (lightOn)
  {
    Serial.println("-");
    digitalWrite(light_pin, HIGH);
    digitalWrite(screen_pin, HIGH);
  }
  else
  {
    digitalWrite(light_pin, HIGH);
    digitalWrite(screen_pin, HIGH);
  }

  delay(500); // Reduz a taxa de atualização
}
