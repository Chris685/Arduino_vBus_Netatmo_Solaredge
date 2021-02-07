// Nutze den Webbrowser von api.netatmo.com für den Fingerpringt SHA1
//const char fingerprint[] PROGMEM = "88c43718e8d8f1d96b4bdc11a1eefe82be676309"; //ersetzt durch client.setInsecure();

//Startwerte / Default, wenn andere Werte in config.json vorhanden sind, werden diese überschrieben!
char client_secret[128] = "";
char client_id[128] = "";
char access_token[128]  = "|";
char refresh_token[128] = "|";
char device_id[128] = "";

void Lese_Einstellungen_Netatmo() {
#ifdef DEBUG_NETATMO
  Serial.println("Nutze ESP8266 Datei System...");
#endif
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
#ifdef DEBUG_NETATMO
    Serial.println("Config Datei von Datei kann nicht System geöffnet werden");
#endif
    return;
  }
  size_t size = configFile.size();
  if (size > 1024) {
#ifdef DEBUG_NETATMO
    Serial.println("Config Datei ist zu groß");
#endif
    return;
  }
  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<346> recall;
  DeserializationError error = deserializeJson(recall, buf.get());
  strcpy(client_secret, recall["client_secret"]);
  strcpy(client_id, recall["client_id"]);
  strcpy(refresh_token, recall["refresh_token"]);
  strcpy(access_token, recall["access_token"]);
  strcpy(device_id, recall["device_id"]);
  if (error)
  {
#ifdef DEBUG_NETATMO
    Serial.println("failed to parse json config");
#endif
    return;
  } else {
#ifdef DEBUG_NETATMO
    Serial.print("Access Token: ");
    Serial.println(access_token);
    Serial.print("Refresh Token: ");
    Serial.println(refresh_token);
    Serial.println("JSON PARSE erfolgreich. \nNutze configFile vom File System...");
#endif
  }
  return;
}

void Speichere_Einstellung_Netatmo()
{
#ifdef DEBUG_NETATMO
  Serial.println("Speichere Einstellung auf ESP8266 Datei System");
#endif
  StaticJsonDocument<346> store;
  store["client_secret"] = client_secret;
  store["client_id"] = client_id;
  store["refresh_token"] = refresh_token;
  store["access_token"] = access_token;
  store["device_id"] = device_id;

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
#ifdef DEBUG_NETATMO
    Serial.println("Config Datei konnte nicht zu speicher geöffnet werden");
#endif
  }
  serializeJson(store, configFile);
  configFile.close();
}



int httpsPostRequest(String s_host, uint16_t httpsPort, String url, String payload, String* ret) {
  char host[256];
  s_host.toCharArray(host, 256);

  WiFiClientSecure client;

  //client.setFingerprint(fingerprint);
  client.setInsecure();
#ifdef DEBUG_NETATMO
  Serial.println("Verbinde mit " + String(host));
#endif
  if (!client.connect(host, httpsPort)) {
#ifdef DEBUG_NETATMO
    Serial.println("HTTPS Verbindung fehlgeschlagen");
#endif
    return -1;
  }
#ifdef DEBUG_NETATMO
  Serial.println("HTTPS Anfrage: " + url);
#endif
  String header = String("POST ") + url + " HTTP/1.0\r\n" +
                  "Host: " + host + "\r\n" +
                  "User-Agent: ESP8266\r\n" +
                  "Content-Type: application/x-www-form-urlencoded\r\n" +
                  "Connection: keep-alive\r\n";
  if (payload.length() != 0) {
    header = header + "Content-Length: " + payload.length() +
             "\r\n\r\n" + payload;
  }
  else {
    header = header + "Content-Length: 0\r\n\r\n";
  }
  client.print(header);
  String responseHeader = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    responseHeader = responseHeader + line;
    if (line == "\r") {
#ifdef DEBUG_NETATMO
      Serial.println("HTTPS Header erfolgreich empfangen");
#endif
      if (!responseHeader.startsWith("HTTP/1.1 200")) {
#ifdef DEBUG_NETATMO
        Serial.println("HTTPS Header ist nicht 200. Header ist :\n" + responseHeader + "\n");
#endif
        client.stop();
        return -1;
      }
      break;
    }
  }
  *ret = client.readStringUntil('\n');

  return 0;
}


int getRefreshToken() {
#ifdef DEBUG_NETATMO
  Serial.println("Abfrage Refresh Token");
#endif
  String str_refresh_token = String(refresh_token);
  str_refresh_token.replace("|", "%7C");
  String data = "client_secret=" + String(client_secret) + "&grant_type=refresh_token&client_id=" + client_id + "&refresh_token=" + str_refresh_token;
  String authCode;
  int ret = httpsPostRequest("api.netatmo.com", 443, "/oauth2/token", data, &authCode);
  if (ret != 0) {
#ifdef DEBUG_NETATMO
    Serial.println("failed get refresh token, https request failed");
#endif
    return ret;
  }


  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(5) + 210;
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, authCode);
#ifdef DEBUG_NETATMO
  Serial.print("Refresh Token: ");
  Serial.println(authCode);
#endif
  if (error) {
#ifdef DEBUG_NETATMO
    Serial.println("Parsing Netatmo Refresh Token fehlgeschlagen");
#endif
    return -1;
  }
#ifdef DEBUG_NETATMO
  Serial.println("Parsing Netatmo Refresh Token erfolgreich");
#endif
  strcpy(access_token, doc["access_token"]);
  strcpy(refresh_token, doc["refresh_token"]);
#ifdef DEBUG_NETATMO
  Serial.print("Neuer Access_token: ");
  Serial.println(access_token);
#endif
  Speichere_Einstellung_Netatmo();
  return 0;
}


int getNetatmoData() {
  int ret = 0;
  String str_temp;
  String str_access_token = String(access_token);
  str_access_token.replace("|", "%7C");
  String str_device_id = String(device_id);
  str_device_id.replace(":", "%3A");

  String params = "?access_token=" + str_access_token + "&device_id=" + str_device_id;
  ret = httpsPostRequest("api.netatmo.com", 443, "/api/getstationsdata" + params, "", &str_temp); //getstationsdata?device_id=70%3Aee%3A50%3A53%3A1d%3Aec&get_favorites=false
  if (ret != 0) {
#ifdef DEBUG_NETATMO
    Serial.println("Netatmo Daten Abfrage fehlgeschlagen");
#endif
    return ret;
  }


  
  const size_t capacity = 3072; //3 * JSON_ARRAY_SIZE(1) + 2 * JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 2 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + 2 * JSON_OBJECT_SIZE(8) + 4 * JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(16) + 1070;
  DynamicJsonDocument doc(capacity);
 DeserializationError error = deserializeJson(doc, str_temp);
  if (error) {
#ifdef DEBUG_NETATMO
    Serial.println("Parsing Neatatmo Daten fehlgeschlagen");
#endif
    return -1;
  }
#ifdef DEBUG_NETATMO
  Serial.println("Parsing Netatmo Daten erfolgreich");
#endif
  //Netatmo
  NexText t_net_innen = NexText (N_Seite, 25, "Netatmo.t_net_innen");
  NexText t_net_aussen = NexText (N_Seite, 26, "Netatmo.t_net_aussen");
  NexText mm_net_1h = NexText (N_Seite, 27, "Netatmo.mm_net_1h");
  NexText mm_net_24h = NexText (N_Seite, 28, "Netatmo.mm_net_24h");
  NexText t_net_co2 = NexText (N_Seite, 31, "Netatmo.t_net_co2");
  NexText t_wind = NexText (N_Seite, 35, "Netatmo.t_wind");
  NexPicture trend_innen = NexPicture(N_Seite, 24, "Netatmo.trend_innen");
  NexPicture trend_aussen = NexPicture(N_Seite, 23, "Netatmo.trend_aussen");
  NexPicture p_CO2 = NexPicture(N_Seite, 32, "Netatmo.p_CO2");
  NexPicture p_Rain = NexPicture(N_Seite, 16, "Netatmo.p_Rain");



  float body_devices_0_dashboard_data_Temperature = doc["body"]["devices"][0]["dashboard_data"]["Temperature"];
  char cNetatmo_Tempinnen[4];
  dtostrf(body_devices_0_dashboard_data_Temperature, 1, 1, cNetatmo_Tempinnen);    // 1 Stelle hinter dem Komma (inklusive Kompensation)
  t_net_innen.setText(cNetatmo_Tempinnen);
/*
  const char* body_devices_0_dashboard_data_temp_trend  = doc["body"]["devices"][0]["dashboard_data"]["temp_trend"];
  if (strcmp(body_devices_0_dashboard_data_temp_trend, "stable") == 0)
  {
    trend_innen.setPic(16);  //16 = stable
  }
  if (strcmp(body_devices_0_dashboard_data_temp_trend, "up") == 0)
  {
    trend_innen.setPic(15);  //15 = up
  }
  if (strcmp(body_devices_0_dashboard_data_temp_trend, "down") == 0)
  {
    trend_innen.setPic(14);  //14 = down
  }
*/

  int body_devices_0_dashboard_data_CO2 = doc["body"]["devices"][0]["dashboard_data"]["CO2"];
  char cNetatmo_CO2[4];
  itoa(body_devices_0_dashboard_data_CO2, cNetatmo_CO2, 10);  // keine Stelle hinter dem Komma
  t_net_co2.setText(cNetatmo_CO2);
  if (body_devices_0_dashboard_data_CO2 >= 2000)
  {
    p_CO2.setPic(19);  //rot
  }
  if (body_devices_0_dashboard_data_CO2 >= 1000)
  {
    p_CO2.setPic(17);  //gelb
  }
  if (body_devices_0_dashboard_data_CO2 < 1000)
  {
    p_CO2.setPic(18);  //grün
  }

  float body_devices_0_modules_0_dashboard_data_Temperature = doc["body"]["devices"][0]["modules"][0]["dashboard_data"]["Temperature"];
  char cNetatmo_Tempaussen[4];
  dtostrf(body_devices_0_modules_0_dashboard_data_Temperature, 1, 1, cNetatmo_Tempaussen);    // 1 Stelle hinter dem Komma
  t_net_aussen.setText(cNetatmo_Tempaussen);

/*
  const char* body_devices_0_modules_0_dashboard_data_temp_trend   = doc["body"]["devices"][0]["modules"][0]["dashboard_data"]["temp_trend"];
  if (strcmp(body_devices_0_modules_0_dashboard_data_temp_trend, "stable") == 0)
  {
    trend_aussen.setPic(16);  //16 = stable
  }
  if (strcmp(body_devices_0_modules_0_dashboard_data_temp_trend, "up") == 0)
  {
    trend_aussen.setPic(15);  //15 = up
  }
  if (strcmp(body_devices_0_modules_0_dashboard_data_temp_trend, "down") == 0)
  {
    trend_aussen.setPic(14);  //14 = down
  }
*/
  float body_devices_0_modules_1_dashboard_data_sum_rain_1 = doc["body"]["devices"][0]["modules"][1]["dashboard_data"]["sum_rain_1"]; // 0
  char cNetatmo_rain1h[4];
  dtostrf(body_devices_0_modules_1_dashboard_data_sum_rain_1, 1, 1, cNetatmo_rain1h);    // 1 Stelle hinter dem Komma (inklusive Kompensation)
  mm_net_1h.setText(cNetatmo_rain1h);
  if (body_devices_0_modules_1_dashboard_data_sum_rain_1 >= 0.1)
    {
    p_Rain.setPic(12);  //12 = Regnet
  }
  else
  {
    p_Rain.setPic(21);  //21 = Regnet nicht
  }

  float body_devices_0_modules_1_dashboard_data_sum_rain_24 = doc["body"]["devices"][0]["modules"][1]["dashboard_data"]["sum_rain_24"]; // 0
  char cNetatmo_rain24h[4];
  dtostrf(body_devices_0_modules_1_dashboard_data_sum_rain_24, 1, 1, cNetatmo_rain24h);    // 1 Stelle hinter dem Komma (inklusive Kompensation)
  mm_net_24h.setText(cNetatmo_rain24h);



  //float body_devices_0_modules_1_dashboard_data_sum_Rain = doc["body"]["devices"][0]["modules"][1]["dashboard_data"]["Rain"];




  int body_devices_0_modules_2_dashboard_data_WindStrength = doc["body"]["devices"][0]["modules"][2]["dashboard_data"]["WindStrength"];
  char cNetatmoWind[4];
  itoa(body_devices_0_modules_2_dashboard_data_WindStrength, cNetatmoWind, 10);  // keine Stelle hinter dem Komma
  t_wind.setText(cNetatmoWind);

#ifdef DEBUG_NETATMO
  Serial.print("  Temperatur Netatmo innen: ");
  Serial.print(body_devices_0_dashboard_data_Temperature);
  Serial.print("  Temperatur Trend innen: ");
//  Serial.print(body_devices_0_dashboard_data_temp_trend);
  Serial.print("  CO2 Netatmo innen: ");
  Serial.println(body_devices_0_dashboard_data_CO2);
  Serial.print("  Temperatur Außen: ");
  Serial.print(body_devices_0_modules_0_dashboard_data_Temperature);
  Serial.print("  Temperatur Trend aussen: ");
//  Serial.println(body_devices_0_modules_0_dashboard_data_temp_trend);
  //Serial.print("  Regnet gerade:  ");
  //Serial.print(body_devices_0_modules_1_dashboard_data_sum_Rain);
  Serial.print("  Regen letzte Stunde: ");
  Serial.print(body_devices_0_modules_1_dashboard_data_sum_rain_1);
  Serial.print("  Regen heute: ");
  Serial.println(body_devices_0_modules_1_dashboard_data_sum_rain_24);
  Serial.print("  Windstärke aktuell: ");
  Serial.println(body_devices_0_modules_2_dashboard_data_WindStrength);
#endif

  return 0;
}



int readNetatmo() {
  int ret = 0;
#ifdef DEBUG_NETATMO
  Serial.println("Abfrage Temperaturdaten Netatmo");
#endif
  ret = getNetatmoData();
  if (ret != 0) {
#ifdef DEBUG_NETATMO
    Serial.println("Netatmo Daten nicht erhalten, nicht möglich Daten abzufragen");
#endif
    ret = getRefreshToken();
    if (ret != 0) {
#ifdef DEBUG_NETATMO
      Serial.println("Netatmo Refresh Token kann nicht erneuert werden");
#endif
    }
    else {
      ret = getNetatmoData();
      if (ret != 0) {
#ifdef DEBUG_NETATMO
        Serial.println("Netatmo Daten nicht erhalten, nicht möglich Daten abzufragen");
#endif
      }
    }
  }
  return 0;
}
