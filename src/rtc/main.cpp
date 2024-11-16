#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <WiFi.h>
#include "time.h"

// const char * ssid = "Dinossauro_Conectado";
// const char * wifipw = "KiwiAmarelo32";

const char * ssid = "NET_2G2931B3";
const char * wifipw = "2F2931B3";

// Variáveis para RTC e NTP
Rtc_Pcf8563 rtc;
bool rtcInitialized = false;
struct tm timeinfo;

// Função para definir o fuso horário
void setTimezone(String timezone){
  Serial.printf("Definindo fuso horário para %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1);
  tzset();
}

// Função para inicializar o NTP
bool initTime(String timezone){
  Serial.println("Configurando o horário via NTP");
  configTime(0, 0,  "a.st1.ntp.br", "b.st1.ntp.br");  // Conecta ao servidor NTP
  if(!getLocalTime(&timeinfo)){
    Serial.println("Falha ao obter o horário via NTP");
    return false;
  }
  Serial.println("Horário obtido via NTP com sucesso");
  setTimezone(timezone);
  return true;
}

// Função para exibir o horário salvo no RTC
void printRtcTime(){
  if (!rtcInitialized){
    Serial.println("Inicializando o RTC...");
    rtc.initClock();
    rtcInitialized = true;
  }

  // Exibir o horário do RTC
  Serial.print("Horário RTC: ");
  Serial.print(rtc.formatTime());
  Serial.print(" ");
  Serial.println(rtc.formatDate(0x01));
}

// Função para configurar o horário no RTC com os dados obtidos via NTP
void setRtcTimeFromNtp(){
  byte day = timeinfo.tm_mday;
  byte weekday = timeinfo.tm_wday;    // tm_wday é o dia da semana (0-6, onde 0 = domingo)
  byte month = timeinfo.tm_mon + 1;   // tm_mon é de 0-11, então precisamos adicionar 1
  byte year = timeinfo.tm_year - 100; // tm_year é o ano desde 1900, então subtraímos 100 para obter o ano desde 2000

  // Exibir os valores que serão configurados no RTC para verificação
  Serial.printf("Definindo no RTC: %02d:%02d:%02d %02d-%02d-%04d\n", 
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, 
                day, month, timeinfo.tm_year + 1900);

  rtc.setDate(day, weekday, month, false, year);
  rtc.setTime(timeinfo.tm_hour-3, timeinfo.tm_min, timeinfo.tm_sec);
  Serial.println("Horário NTP definido no RTC");
}

// Função para iniciar o Wi-Fi com timeout de 30 segundos
void startWifi(){
  WiFi.begin(ssid, wifipw);
  Serial.println("Conectando ao Wi-Fi");

  unsigned long startAttemptTime = millis();  // Marca o início da tentativa de conexão

  // Continua tentando conectar enquanto não estiver conectado e não passar o timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) {
    Serial.print(".");
    delay(500);
  }

  // Verifica se conectou ou se o timeout foi atingido
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao Wi-Fi.");
  } else {
    Serial.println("\nFalha ao conectar ao Wi-Fi. Continuando sem Wi-Fi.");
  }


}

// Função setup
void setup(){


  
  Serial.begin(9600);
  startWifi();


  initTime("<00>");
  rtc.initClock();

  // Tenta obter o horário via NTP e definir o RTC
  if(WiFi.status() == WL_CONNECTED) {   // Definir fuso horário (América do Sul)
    setRtcTimeFromNtp();      // Define o horário do RTC com os dados NTP
  } else {
    Serial.println("Usando o horário atual do RTC.");
  }

  


}

// Função loop
void loop(){
  // No loop, o horário é sempre lido do RTC e exibido no serial
  Serial.print("Horário RTC: ");
  Serial.print(rtc.formatTime());
  Serial.print(" ");
  Serial.println(rtc.formatDate(0x01));
  
  delay(1000);  // Atraso de 1 segundo
}
