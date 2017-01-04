void initBME280() {
  Serial.println(F("Temperatursensor BME280 gestartet"));

  if (!bme.begin()) {
    Serial.println("Kann keinen BME280 sensor finden!");
    while (1);
  }

}

void readBME280() {
    /*
    Serial.print("Temperatur = ");
    Serial.print(bme.readTemperature());
    Serial.println(" C");
    Serial.print("Druck = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
    Serial.print("Feuchtigkeit = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    */
    // Umwandlung Float in Char zur Anzeige von Fließpunktkomma auf Nextion
    char cTemp[4];
    char cFeuchtigkeit[4];
    char cDruck[4];
    dtostrf(bme.readTemperature(),1,1,cTemp);         // 1 Stelle hinter dem Komma
    dtostrf(bme.readHumidity(),1,1,cFeuchtigkeit);    // 1 Stelle hinter dem Komma
    dtostrf(bme.readPressure() / 100.0F,0,0,cDruck);  // keine Stelle hinter dem Komma

    // Ausgabe auf Nextion Display
    Temp.setText(cTemp);
    Humity.setText(cFeuchtigkeit);
    Druck.setText(cDruck);   
}


