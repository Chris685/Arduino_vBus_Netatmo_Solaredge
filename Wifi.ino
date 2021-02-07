void initWifi() {
  delay (300);
  //SSID von Nextion
  char cNexSSID [20];
  memset(cNexSSID, 0, sizeof(cNexSSID));
  NexSSID.getText(cNexSSID, sizeof(cNexSSID));
  //Passwort von Nextion
  char cNexpass [20];
  memset(cNexpass, 0, sizeof(cNexpass));
  Nexpass.getText(cNexpass, sizeof(cNexpass));
#ifdef DEBUG_WIFI
  Serial.print("Verbinde zu Netzwerk: ");
  Serial.print(cNexSSID);
#endif

  WiFi.mode(WIFI_STA);
  IPAddress ip(192, 168, 1, 27);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 1, 1);
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(cNexSSID, cNexpass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG_WIFI
    Serial.print(".");
#endif
  }

#ifdef DEBUG_WIFI
  Serial.println(" Verbunden mit Netzwerk \n");
#endif

}
