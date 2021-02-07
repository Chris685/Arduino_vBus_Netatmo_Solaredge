// Konfiguration OTA
const char* host = "Temperaturanzeige";
const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "Coburg_1";
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;


void initOTA() {
  MDNS.begin(host);

  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
#ifdef DEBUG_OTA
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", host, update_path, update_username, update_password);
#endif
}

void OTA() {
  httpServer.handleClient();
  MDNS.update();
}
