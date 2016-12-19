bool connect(const char* hostName) {
  Serial.print("Connect to ");
  Serial.println(hostName);

  bool ok = client.connect(hostName, 80);

  Serial.println(ok ? "Verbunden mit Resol DL2" : "Verbindung fehlgeschlagen!");
  return ok;
}

// Send the HTTP GET request to the server
bool sendRequest(const char* host, const char* resource) {
  Serial.print("Frage Daten von Resol DL2 ab: ");
  Serial.println(resource);
  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.0");
  client.print("Host: ");
  client.println(server);
  client.println("Connection: close");
  client.println();

  return true;
}

// Skip HTTP headers so that we are at the beginning of the response's body
bool skipResponseHeaders() {
   //HTTP headers end with an empty line
   char endOfHeaders[] = "\r\n\r\n";

   client.setTimeout(HTTP_TIMEOUT);
   bool ok = client.find(endOfHeaders);

   if (!ok) {
     Serial.println("Keine oder falsche Antwort von Resol DL2!");
   }

   return ok;
}

// Read the body of the response from the HTTP server
void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.readBytes(content, maxSize);
  content[length] = 0;
  //Serial.println(content);
}

bool parsevBus(char* content, struct vBus* userData) {
  // Berechnung unter http://jsonviewer.stack.hu/
  //const size_t BUFFER_SIZE =
  //   3*JSON_ARRAY_SIZE(1)     // the root object has 1 elements
  //    + 2*JSON_OBJECT_SIZE(25)   // the "headers" object has 5 elements
  //    + JSON_OBJECT_SIZE(2)   // the "headerset_stats" object has 2 elements
  //    + 27*JSON_OBJECT_SIZE(3);  // the "headersets" object has 3 elements
  //    + 26*JSON_ARRAY_SIZE(4);
  //    + JSON_OBJECT_SIZE(11);

  //Allocate a temporary memory pool on the stack
  //StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  //If the memory pool is too big for the stack, use this instead:
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
  Serial.print("Zeit: ");
  Serial.println(vbus->Zeit);

  unsigned int Pumpe_Fest = atoi(vbus->Pumpe_Festbrennstoff);
  unsigned int Pumpe_prim = atoi(vbus->Pumpe_primaer);

  speicher_oben.setText(vbus->Speicher_oben);
  speicher_mitte.setText(vbus->Speicher_mitte);
  speicher_unten.setText(vbus->Speicher_unten);
  festbrennstoff.setText(vbus->Festbrennstoff);
  kollektor.setText(vbus->Kollektor);
  JSONZeit.setText(vbus->Zeit);



  if (Pumpe_Fest >= 1) { 
    Background.Set_background_image_pic(1);
    Serial.println("Feuer");
  }
  if (Pumpe_prim >= 1) {
    Background.Set_background_image_pic(3);
    Serial.println("Solar");
  }
  if ((Pumpe_Fest >= 1) && (Pumpe_prim >= 1)) {
    Background.Set_background_image_pic(2);
    Serial.println("Feuer_Solar");
  }
  if ((Pumpe_Fest < 1) && (Pumpe_prim < 1)) {
    Background.Set_background_image_pic(0);
    Serial.println("leer");
  }
}
void readJSON() {
     if (connect(server)) {
        if (sendRequest(server, resource) && skipResponseHeaders() ) {    //Lese vBus JSON Daten
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
