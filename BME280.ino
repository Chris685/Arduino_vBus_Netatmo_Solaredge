// Konfiguration Temperatursensor
Adafruit_BME280 bme;                    // Verbindung ueber I2C
float calckompensation = 0;             // Aufheizkompensation. Parameter kommen über Display


void initBME280() {
#ifdef DEBUG_BME280
  Serial.println(F("Temperatursensor BME280 intialisiert"));
#endif

  if (!bme.begin(0x77, &Wire)) {
#ifdef DEBUG_BME280
    Serial.println(F("Kann keinen BME280 sensor finden!"));
#endif
    //while (1);      **Absturz wegen SW-Watchdog**
  }
#ifdef DEBUG_BME280
  Serial.println("-- Wetter Station --");
  Serial.println("forced mode, 1x Temperatur / 1x Feuchtigkeit / Druck aus / Filter aus");
#endif

  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // Temperatur
                  Adafruit_BME280::SAMPLING_NONE, // Druck
                  Adafruit_BME280::SAMPLING_X1, // Feuchtigkeit
                  Adafruit_BME280::FILTER_OFF   ); // Filter

}

void readBME280() {
#ifdef DEBUG_BME280
  Serial.println(F("Temperatursensor BME280 auslesen"));
#endif
  //Berechnung der Kompensation abzüglich der Starterwärmung
  //Abholen des Offsets vom Display zur Kompensation
  char cTemperatur [20];
  memset(cTemperatur, 0, sizeof(cTemperatur));
  NexTemperatur.getText(cTemperatur, sizeof(cTemperatur));
  float kompensation = atof (cTemperatur);
  //Abholen der Aufheizzeit vom Display zur Kompensation
  char cZeit [20];
  memset(cZeit, 0, sizeof(cZeit));
  NexZeit.getText(cZeit, sizeof(cZeit));
  long timekompensation = atof (cZeit) * 60 * 1000;

  if (timekompensation <= 0) {
    calckompensation = kompensation;
  }
  else {
    //Berechnung der Aufheizkompensation BME280
    if (calckompensation < kompensation) {
      calckompensation = calckompensation + (kompensation / (timekompensation / ms30000));
#ifdef DEBUG_BME280
      Serial.print(F("Offset in Crad von Display = "));
      Serial.print(kompensation);
      Serial.print(F(" / Zeit für Kompensation = "));
      Serial.println(timekompensation);
      Serial.print(F("kalkulierte Temperaturkompensation um= "));
      Serial.println(calckompensation);
#endif
    }
  }
  bme.takeForcedMeasurement();

#ifdef DEBUG_BME280
  Serial.print("Temperatur = ");
  Serial.print(bme.readTemperature());
  Serial.print(" C  /  ");
  Serial.print("Temperatur kompensiert = ");
  Serial.print(bme.readTemperature() - calckompensation);
  Serial.println(" C");
  //Serial.print("Druck = ");
  //Serial.print(bme.readPressure() / 100.0F);
  //Serial.println(" hPa");
  Serial.print("Feuchtigkeit = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
#endif
  // Umwandlung Float in Char zur Anzeige von Fließpunktkomma auf Nextion
  char cTemp[4];
  char cFeuchtigkeit[4];
  //char cDruck[4];
  dtostrf(bme.readHumidity(), 1, 1, cFeuchtigkeit); // 1 Stelle hinter dem Komma
  dtostrf((bme.readTemperature() - calckompensation), 1, 1, cTemp);    // 1 Stelle hinter dem Komma (inklusive Kompensation)
  //dtostrf(bme.readPressure() / 100.0F,0,0,cDruck);  // keine Stelle hinter dem Komma

  //BME280
  NexText Temp = NexText(N_Seite, 30, "Startseite.Temp");
  NexText Humity = NexText(N_Seite, 31, "Startseite.Humity");
  //NexText Druck = NexText(N_Seite, 3, "Startseite.Druck");

  // Ausgabe auf Nextion Display
  Humity.setText(cFeuchtigkeit);
  Temp.setText(cTemp);
  //Druck.setText(cDruck);
#ifdef DEBUG_BME280
  Serial.println("BME280 beendet");
#endif
}
