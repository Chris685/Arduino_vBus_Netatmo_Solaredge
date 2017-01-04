// Hole Adresse von Nextion Display
void initJSON(){
  memset(cNexadresse, 0, sizeof(cNexadresse));
  Nexadresse.getText(cNexadresse,sizeof(cNexadresse));   
}

// Verbinde zu Server
bool connect(const char* cNexadresse) {
  Serial.print("Verbinde zu ");
  Serial.println(cNexadresse);

  bool ok = client.connect(cNexadresse, 80);

  Serial.println(ok ? "Verbunden mit Resol DL2" : "Verbindung fehlgeschlagen!");
  return ok;
}

// Sende die HTTP GET anfoderung zum Server
bool sendRequest(const char* cNexadresse, const char* resource) {
  Serial.print("Frage Daten von Resol DL2 ab: ");
  Serial.println(resource);
  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.0");
  client.print("Host: ");
  client.println(cNexadresse);
  client.println("Connection: close");
  client.println();

  return true;
}

// Verwerfe den HTTP Header um mit den Daten zu starten
bool skipResponseHeaders() {
// HTTP Header endet mit einer leeren Zeile
   char endOfHeaders[] = "\r\n\r\n";

   client.setTimeout(HTTP_TIMEOUT);
   bool ok = client.find(endOfHeaders);

   if (!ok) {
     Serial.println("Keine oder falsche Antwort von Resol DL2!");
   }

   return ok;
}

// Auslsen der Antwort vom Server
void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.readBytes(content, maxSize);
  content[length] = 0;
  //Serial.println(content);        // Ausgabe der JSON Tabelle auf serieller Konsole
}

bool parsevBus(char* content, struct vBus* userData) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(content);

  if (!root.success()) {
    Serial.println("JSON parsing fehlgeschlagen!");
    return false;
  }

  // Hier die Werte zum decyrpten
  strcpy(userData->Kollektor, root["headersets"][0]["packets"][0]["field_values"][0]["value"]);
  strcpy(userData->Waermetauscher, root["headersets"][0]["packets"][0]["field_values"][1]["value"]);
  strcpy(userData->Speicher_unten, root["headersets"][0]["packets"][0]["field_values"][2]["value"]);
  strcpy(userData->Speicher_oben, root["headersets"][0]["packets"][0]["field_values"][3]["value"]);
  strcpy(userData->Speicher_mitte, root["headersets"][0]["packets"][0]["field_values"][4]["value"]);
  strcpy(userData->Festbrennstoff, root["headersets"][0]["packets"][0]["field_values"][7]["value"]);
  strcpy(userData->Pumpe_primaer, root["headersets"][0]["packets"][0]["field_values"][12]["value"]);
  strcpy(userData->Pumpe_sekundaer, root["headersets"][0]["packets"][0]["field_values"][13]["value"]);
  strcpy(userData->Pumpe_Festbrennstoff, root["headersets"][0]["packets"][0]["field_values"][15]["value"]);
  strcpy(userData->Zeit, root["headersets"][0]["packets"][0]["field_values"][23]["value"]);
  return true;
}

// Gebe vBus Daten aus
void printvBus(const struct vBus* vbus) {
  /*
  Serial.print("Kollektor: ");
  Serial.println(vbus->Kollektor);
  Serial.print("Waermetauscher: ");
  Serial.println(vbus->Waermetauscher);
  Serial.print("Speicher_oben: ");
  Serial.println(vbus->Speicher_oben);
  Serial.print("Speicher_mitte: ");
  Serial.println(vbus->Speicher_mitte);
  Serial.print("Speicher_unten: ");
  Serial.println(vbus->Speicher_unten);
  Serial.print("Festbrennstoff: ");
  Serial.println(vbus->Festbrennstoff);
  Serial.print("Pumpe primaer: ");
  Serial.println(vbus->Pumpe_primaer);
  Serial.print("Pumpe sekundaer: ");
  Serial.println(vbus->Pumpe_sekundaer);
  Serial.print("Pumpe Festbrennstoff: ");
  Serial.println(vbus->Pumpe_Festbrennstoff);
  Serial.print("Zeit Vbus: ");
  Serial.println(vbus->Zeit);
  */
  Serial.println("JSON parsing erfolgreich!");
  // Char in Int zur weiterern Berechnung
  unsigned int Pumpe_Fest = atoi(vbus->Pumpe_Festbrennstoff);
  unsigned int Pumpe_prim = atoi(vbus->Pumpe_primaer);
  
  // Ausgabe auf Nextion Display
  speicher_oben.setText(vbus->Speicher_oben);
  speicher_mitte.setText(vbus->Speicher_mitte);
  speicher_unten.setText(vbus->Speicher_unten);
  festbrennstoff.setText(vbus->Festbrennstoff);
  kollektor.setText(vbus->Kollektor);

   // Bilder zur Visualisierung
  if (Pumpe_Fest >= 1) {            // Bild Feuer
    //Background.Set_background_image_pic(1);
    p0.setPic(1);
  }
  if (Pumpe_prim >= 1) {            // Bild Sonne
    //Background.Set_background_image_pic(3);
    p0.setPic(3);
  }                                 // Bilder Feuer und Sonne
  if ((Pumpe_Fest >= 1) && (Pumpe_prim >= 1)) {
    //Background.Set_background_image_pic(2);
    p0.setPic(2);
  }                                 // Bild leer
  if ((Pumpe_Fest < 1) && (Pumpe_prim < 1)) {
    //Background.Set_background_image_pic(0);
    p0.setPic(4);
  }
}
void readJSON() {
     if (connect(cNexadresse)) {
        if (sendRequest(cNexadresse, resource)&& skipResponseHeaders()) {    //Lese vBus JSON Daten
          char response[MAX_CONTENT_SIZE];
          readReponseContent(response, sizeof(response));
           vBus vbus;
            if (parsevBus(response, &vbus)) {
            printvBus(&vbus);
            }
          }
         disconnect();
    }
}

void disconnect() {
  Serial.println("Verbindung zu Resol DL2 getrennt");
  client.stop();
}
