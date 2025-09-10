#include "ESP32WebConfig.h"

// Crear instancia
ESP32WebConfig webConfig("TP-LINK_2FE3C4", "87718508", "admin", "1234");

void setup() {
  webConfig.begin();
}

void loop() {
  webConfig.handleClient();
}
