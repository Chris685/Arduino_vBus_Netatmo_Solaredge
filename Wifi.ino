void initWifi() {
  delay (300);
  if (WiFi.status() == WL_NO_SHIELD){
  Serial.println("Kann kein WiFi Shield finden!");
    // don't continue:
    while (true);
  }
  while (status != WL_CONNECTED) {
    //SSID von Nextion
    char cNexSSID [20];
    memset(cNexSSID, 0, sizeof(cNexSSID));
    NexSSID.getText(cNexSSID,sizeof(cNexSSID));
    //Passwort von Nextion
    char cNexpass [20];
    memset(cNexpass, 0, sizeof(cNexpass));
    Nexpass.getText(cNexpass,sizeof(cNexpass));
    Serial.print("Verbinde zu Netzwerk: ");
    Serial.println(cNexSSID);
    status = WiFi.begin(cNexSSID, cNexpass);
    delay(2000);
    }
  Serial.println("Verbunden mit Netzwerk");
}


