// Hole Adresse von Nextion Display
void initvBUS() {
#ifdef DEBUG_vBUS
  Serial.println(F("JSON vBUS initalisieren"));
#endif
  memset(cNexadresse, 0, sizeof(cNexadresse));
  Nexadresse.getText(cNexadresse, sizeof(cNexadresse));
}
void readvBUS() {
#ifdef DEBUG_vBUS
  Serial.println(F("JSON vBUS starten"));
#endif
  // Verbinde zu Server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;  //Object of class HTTPClient
    char dl2_adresse[sizeof("http://") + sizeof(cNexadresse) + sizeof(resource)];
    memset(dl2_adresse, 0, sizeof(dl2_adresse));
    strcat(dl2_adresse, "http://");
    strcat(dl2_adresse, cNexadresse);
    strcat(dl2_adresse, resource);
#ifdef DEBUG_vBUS
    Serial.println(dl2_adresse);
#endif
    http.begin(dl2_adresse);
    int httpCode = http.GET();
    //String payload = http.getString();
#ifdef DEBUG_vBUS
    Serial.print("Response Code: "); //200 is OK
    Serial.println(httpCode);
    //Serial.print("Antwort vom Server:");
    //Serial.println(payload);
#endif

    if (httpCode == 200) {
      // Parsing
#ifdef DEBUG_vBUS
      Serial.println("Syntaxanalyse JSON vBUS");
#endif
      const size_t capacity = 3 * JSON_ARRAY_SIZE(1) + 2 * JSON_ARRAY_SIZE(25) + JSON_OBJECT_SIZE(2) + 27 * JSON_OBJECT_SIZE(3) + 26 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(11) + 2929;
      DynamicJsonDocument doc(capacity);

      DeserializationError error = deserializeJson(doc, http.getString());

      if (error) {
#ifdef DEBUG_vBUS
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
#endif
        return;
      }
      // JSON Variablen

      const char* vBusKollektor = doc["headersets"][0]["packets"][0]["field_values"][0]["value"];
      const char* vBusSpeicher_oben = doc["headersets"][0]["packets"][0]["field_values"][3]["value"];
      const char* vBusSpeicher_mitte = doc["headersets"][0]["packets"][0]["field_values"][4]["value"];
      const char* vBusSpeicher_unten = doc["headersets"][0]["packets"][0]["field_values"][2]["value"];
      const char* vBusFestbrennstoff = doc["headersets"][0]["packets"][0]["field_values"][7]["value"];
      const char* vBusPumpe_primaer = doc["headersets"][0]["packets"][0]["field_values"][12]["value"];
      const char* vBusPumpe_Festbrennstoff = doc["headersets"][0]["packets"][0]["field_values"][15]["value"];
#ifdef DEBUG_vBUS
      Serial.println("Ausgabe JSON vBUS Daten");
#endif
      //VBUS
      NexText speicher_oben = NexText(N_Seite, 1, "Startseite.speicher_oben");
      NexText speicher_mitte = NexText(N_Seite, 2, "Startseite.speicher_mitte");
      NexText speicher_unten = NexText(N_Seite, 3, "Startseite.speicher_unten");
      NexText festbrennstoff = NexText(N_Seite, 28, "Startseite.festbrennstoff");
      NexText kollektor = NexText(N_Seite, 29, "Startseite.kollektor");
      //NexPicture p_feuer = NexPicture(N_Seite, 1, "Startseite.p_feuer");
      NexPicture p_kollektor = NexPicture(N_Seite, 1, "Startseite.p_kollektor");



      kollektor.setText(vBusKollektor);
      speicher_oben.setText(vBusSpeicher_oben);
      speicher_mitte.setText(vBusSpeicher_mitte);
      speicher_unten.setText(vBusSpeicher_unten);
      festbrennstoff.setText(vBusFestbrennstoff);

      unsigned int Pumpe_Fest = atoi(vBusPumpe_Festbrennstoff);
      unsigned int Pumpe_prim = atoi(vBusPumpe_primaer);


      // Bilder zur Visualisierung
      if ((Pumpe_Fest >= 1) && (Pumpe_prim == 0)) {           // Bild Feuer, Rauch
        //p_feuer.setPic(7);              // Feuer im Kamin
        p_kollektor.setPic(6);          // Dach mit Rauch
      }
      if ((Pumpe_prim >= 1) && (Pumpe_Fest == 0)) {            // Bild Sonne
        //p_feuer.setPic(6);              // Kamin
        p_kollektor.setPic(4);          // Dach mit Sonne
      }
      if ((Pumpe_prim >= 1) && (Pumpe_Fest >= 1)) {             // Bild Feuer, Sonne, Rauch
        //p_feuer.setPic(7);              // Feuer im Kamin
        p_kollektor.setPic(5);          // Dach mit Sonne und Rauch
      }
      if ((Pumpe_Fest == 0) && (Pumpe_prim == 0)) {           // Bild Standart
        //p_feuer.setPic(6);              // Kamin
        p_kollektor.setPic(3);          // Dach
      }
    }
    else
    {
      Serial.println("Fehler in response");
      return;
    }
    http.end();
#ifdef DEBUG_vBUS
    Serial.println("JSON vBUS abgeschlossen - Verbindung beendet");
#endif
  }
  else {
#ifdef DEBUG_vBUS
    Serial.println("WIFI nicht verfügbar");
#endif
  }
}
