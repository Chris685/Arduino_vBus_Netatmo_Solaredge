void initWifi() {
  if (WiFi.status() == WL_NO_SHIELD){
  Serial.println("WiFi shield nicht gesteckt");
    // don't continue:
    while (true);
  }
  while (status != WL_CONNECTED) {
    Serial.print("Verbinde zu Netzwerk: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
    }
  Serial.println("Verbunden mit Netzwerk");
}


