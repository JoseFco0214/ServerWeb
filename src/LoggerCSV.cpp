#include "LoggerCSV.h"

LoggerCSV::LoggerCSV(const char* file) {
  filename = String(file);
}

bool LoggerCSV::begin() {
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Error montando SPIFFS");
    return false;
  }

  if (!SPIFFS.exists(filename)) {
    clear(); // crea archivo con encabezado
  }
  return true;
}

void LoggerCSV::addRecord(int value) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("⚠️ No se pudo obtener la hora");
    return;
  }

  char fechaHora[30];
  strftime(fechaHora, sizeof(fechaHora), "%Y-%m-%d %H:%M:%S", &timeinfo);

  File file = SPIFFS.open(filename, FILE_APPEND);
  if (!file) {
    Serial.println("❌ No se pudo abrir el archivo para escribir");
    return;
  }

  file.printf("%d,%s\n", value, fechaHora);
  file.close();

  Serial.printf("✅ Guardado: %d,%s\n", value, fechaHora);
}

void LoggerCSV::readAll() {
  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    Serial.println("❌ No se pudo abrir el archivo para leer");
    return;
  }

  Serial.println("📂 Contenido de registros:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void LoggerCSV::clear() {
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("❌ No se pudo abrir el archivo para limpiar");
    return;
  }
  file.println("valor,fecha_hora");
  file.close();
  Serial.println("🗑️ Archivo reiniciado con encabezado");
}
