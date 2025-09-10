#include "ESP32WebConfig.h"
#include "LoggerCSV.h"

#include <Wifi.h>

// 🔹 Config WiFi
const char* ssid = "TP-LINK_2FE3C4";
const char* password = "87718508";

// 🔹 Config NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Crear instancia
ESP32WebConfig webConfig("ESP32-1","1234",ssid, password, "admin", "1234");


// variables globales
LoggerCSV logger("/datos.csv");
unsigned long lastSave = 0;
int counter = 0;

// ---------------------------------------------------------
void ConexionWifi(){
    // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
}


// ---------------------------------------------------------
void setup() {
//  webConfig.begin();
  Serial.begin(115200);

 // Conectar WIFI 
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");

// Configurar NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
/*
  setenv("TZ", "CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  configTime(0, 0, "pool.ntp.org");
*/
  // Inicializar logger
  if (logger.begin()) {
    Serial.println("Logger listo ✅");
  } else {
    Serial.println("Error al inicializar el logger ❌");
  }

}

// ---------------------------------------------------------
void loop() {
//  webConfig.handleClient();

unsigned long now = millis();

  if (now - lastSave >= 5000) {  // cada 5 segundos
    lastSave = now;
    counter++;
    logger.addRecord(counter);
  }



  if (counter == 10) { // ejemplo: limpiar cuando llegamos a 10
     // Lectura completa de registros
     logger.readAll();

     // Limpieza
    logger.clear();
    counter = 0;
  }

}
