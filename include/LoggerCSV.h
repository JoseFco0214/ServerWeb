#ifndef LOGGERCSV_H
#define LOGGERCSV_H

#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include "time.h"

class LoggerCSV {
  private:
    String filename;

  public:
    LoggerCSV(const char* file);
    bool begin();
    void addRecord(int value);
    void readAll();
    void clear();
};

#endif
