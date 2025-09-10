#include "ESP32WebConfig.h"

ESP32WebConfig::ESP32WebConfig(const char* fssidAP, const char* fpassAP, const char* fwifiSSID, 
                   const char* fwifiPASS, const char* user, const char* pwd)
  : server(80) {
  ssidAP = fssidAP;
  passAP = fpassAP;
  wifiSSID = fwifiSSID;
  wifiPASS = fwifiPASS;

  authUser = user;
  authPass = pwd;
}

void ESP32WebConfig::begin() {

  Serial.begin(115200);
 
  loadPreferences();

  local_IP.fromString(ipStr);
  subnet.fromString(maskStr);
  gateway.fromString(gatewayStr);

  // Intentar modo STA
  startSTAMode();

  // Modo AP
//    startAPMode();

}

void ESP32WebConfig::handleClient() {
  server.handleClient();
}

// ------------------------------------------------------------ APMODE
void ESP32WebConfig::startAPMode() {
    // Iniciar AP
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssidAP, passAP);

  Serial.println("Iniciando en modo AP...");
  Serial.println("SSIDAP :"+String(ssidAP));
  Serial.println("PASSAP :"+String(passAP));
  Serial.println(WiFi.softAPIP());

  // Rutas del servidor
  server.on("/", std::bind(&ESP32WebConfig::handleRoot, this));
  server.on("/save", HTTP_POST, std::bind(&ESP32WebConfig::handleSave, this));
  server.on("/restart", HTTP_GET, std::bind(&ESP32WebConfig::handleRestart, this));

  server.begin();
  Serial.println("Servidor HTTP iniciado");


}
// ------------------------------------------------------------ STAMODE
void ESP32WebConfig::startSTAMode() {
  Serial.println("Iniciando en modo STA...");

  WiFi.mode(WIFI_STA);

  // Usar IP fija
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("⚠️ Error al configurar IP estática, usando DHCP...");
  }

  Serial.println("Conectando a wifiSSID : " + wifiSSID);
  WiFi.begin(wifiSSID, wifiPASS);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ Conectado a WiFi con IP: " + WiFi.localIP().toString());

    // Rutas
    server.on("/", std::bind(&ESP32WebConfig::handleRoot, this));
    server.on("/save", HTTP_POST, std::bind(&ESP32WebConfig::handleSave, this));
    server.on("/restart", HTTP_GET, std::bind(&ESP32WebConfig::handleRestart, this));

    server.begin();
    Serial.println("Servidor HTTP en modo STA iniciado");
  } 


}

// ------------------------------------------------------------
bool ESP32WebConfig::isAuthenticated() {
  if (!server.authenticate(authUser, authPass)) {
    server.requestAuthentication();
    return false;
  }
  return true;

}
// ------------------------------------------------------------
void ESP32WebConfig::handleRoot() {
    if (!isAuthenticated()) return;

   
   
  Serial.println("handleRoot()");      
  Serial.println("SSID: " + String(wifiSSID));
  Serial.println("PASS: " + String(wifiPASS));
  Serial.println("IP: " + ipStr);
  Serial.println("MASK: " + maskStr);
  Serial.println("GATEWAY: " + gatewayStr);

String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="UTF-8">
    <title>Configuración de Red ESP32</title>
    <style>
      body { font-family: Arial, sans-serif; background-color: #f2f2f2;
             display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
      .container { background-color: white; padding: 30px; border-radius: 12px;
                   box-shadow: 0 4px 8px rgba(0,0,0,0.2); width: 350px; text-align: center; }
      h1 { font-size: 20px; margin-bottom: 20px; }
      input[type=text], input[type=password] { width: 100%; padding: 10px; margin: 8px 0; border: 1px solid #ccc; border-radius: 6px; }
      input[type=submit] { background-color: #4CAF50; color: white; padding: 12px;
                           border: none; border-radius: 6px; cursor: pointer; width: 100%; font-size: 16px; }
      input[type=submit]:hover { background-color: #45a049; }
      label { display:block; margin-top:10px; text-align:left; }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>Configuración de Red</h1>
      <form action='/save' method='POST'>
        <label>SSID WiFi:</label>
        <input type='text' name='ssid' value=')rawliteral" + wifiSSID + R"rawliteral(' required>
        <label>Password:</label>
        <input type='password' name='pass' value=')rawliteral" + wifiPASS + R"rawliteral(' required>
        <label>IP:</label>
        <input type='text' name='ip' value=')rawliteral" + ipStr + R"rawliteral(' required>
        <label>Máscara:</label>
        <input type='text' name='mask' value=')rawliteral" + maskStr + R"rawliteral(' required>
        <label>Gateway:</label>
        <input type='text' name='gateway' value=')rawliteral" + gatewayStr + R"rawliteral(' required>
        <input type='submit' value='Guardar'>
      </form>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// ------------------------------------------------------------
void ESP32WebConfig::handleSave() {
  if (!isAuthenticated()) return;

 // wifiSSID = server.arg("ssid");
 // wifiPASS = server.arg("pass");
  ipStr = server.arg("ip");
  maskStr = server.arg("mask");
  gatewayStr = server.arg("gateway");

  
  Serial.println("handleSave()");
//  Serial.println("SSID: " + wifiSSID);
//  Serial.println("PASS: " + wifiPASS);
  Serial.println("IP: " + ipStr);

  savePreferences();

  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="UTF-8">
    <title>Configuración Guardada</title>
    <style>
      body { font-family: Arial, sans-serif; background-color: #f2f2f2;
             display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
      .container { background-color: white; padding: 30px; border-radius: 12px;
                   box-shadow: 0 4px 8px rgba(0,0,0,0.2); width: 400px; text-align: center; }
      h1 { font-size: 22px; color: #4CAF50; margin-bottom: 20px; }
      p { font-size: 16px; margin: 8px 0; }
      .config-box { text-align: left; background: #f9f9f9; border: 1px solid #ddd; border-radius: 6px;
                    padding: 15px; margin: 20px 0; }
      .config-box strong { display: inline-block; width: 120px; }
      a, button { display: inline-block; padding: 12px 20px; background-color: #2196F3;
                  color: white; text-decoration: none; border: none; border-radius: 6px;
                  font-size: 16px; cursor: pointer; margin-top: 10px; }
      a:hover, button:hover { background-color: #0b7dda; }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>¡Configuración Guardada!</h1>
      <p>Los nuevos parámetros de red han sido almacenados:</p>
      <div class="config-box">
        <p><strong>SSID:</strong> )rawliteral" + wifiSSID + R"rawliteral(</p>
        <p><strong>Password:</strong> ****</p>
        <p><strong>IP:</strong> )rawliteral" + ipStr + R"rawliteral(</p>
        <p><strong>Máscara:</strong> )rawliteral" + maskStr + R"rawliteral(</p>
        <p><strong>Gateway:</strong> )rawliteral" + gatewayStr + R"rawliteral(</p>
      </div>
      <a href='/'>Volver</a><br>
      <button onclick="fetch('/restart').then(()=>alert('Reiniciando ESP32...'));">Reiniciar ESP32</button>
    </div>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void ESP32WebConfig::handleRestart() {
  server.send(200, "text/plain", "Reiniciando...");
  delay(200);
  ESP.restart();
}

void ESP32WebConfig::loadPreferences() {
  Serial.println("loadPreferences()");

  preferences.begin("network", true);
//  wifiSSID = preferences.getString("ssid", "");
//  wifiPASS = preferences.getString("pass", "");
  ipStr = preferences.getString("ip", "192.168.1.50");
  maskStr = preferences.getString("mask", "255.255.255.0");
  gatewayStr = preferences.getString("gateway", "192.168.1.1");
  preferences.end();



}

void ESP32WebConfig::savePreferences() {
  Serial.println("savePreferences()");

  preferences.begin("network", false);
  preferences.putString("ssid", wifiSSID);
  preferences.putString("pass", wifiPASS);
  preferences.putString("ip", ipStr);
  preferences.putString("mask", maskStr);
  preferences.putString("gateway", gatewayStr);
  preferences.end();
}
