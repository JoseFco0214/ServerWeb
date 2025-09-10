#ifndef ESP32WEBCONFIG_H
#define ESP32WEBCONFIG_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <functional>

class ESP32WebConfig {
  public:
    ESP32WebConfig(const char* fssidAP = "ESP32_Config", 
                   const char* fpassAP = "12345678", 
                   const char* fwifiSSID = "SSID_WiFi", 
                   const char* fwifiPASS = "PASS_WiFi", 
                   const char* user = "admin", 
                   const char* pwd = "1234");

    void begin();
    void handleClient();

  private:
    Preferences preferences;
    WebServer server;
    String ipStr, maskStr, gatewayStr;
    IPAddress local_IP, subnet, gateway;

    const char* ssidAP;
    const char* passAP;
    const char* authUser;
    const char* authPass;
    String wifiSSID;
    const char* wifiPASS;


    bool isAuthenticated();
    void handleRoot();
    void handleSave();
    void handleRestart();
    void loadPreferences();
    void savePreferences();
    void startAPMode();
    void startSTAMode();
};

#endif
