

void MQTT () {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    // Check if its the Zisterne_Liter feed
    if (subscription == &Zisterne_Liter) {
#ifdef DEBUG_MQTT
      Serial.print(F("Zisterne_Liter (mit Kommastellen): "));
      Serial.println((char *)Zisterne_Liter.lastread);
#endif
      //MQTT
      NexText Liter = NexText(N_Seite, 45, "Startseite.Liter");
      unsigned int iLiter = atoi((char *)Zisterne_Liter.lastread);    //Char to Integer
      char cLiter[4];
      dtostrf(iLiter, 0, 0, cLiter); // keine Stelle hinter dem Komma     //Integer to Char (um Kommastellen zu streichen)
#ifdef DEBUG_MQTT
      Serial.print(F("Zisterne_Liter (ohne Kommastellen): "));
      Serial.println(cLiter);
      Serial.println("MQTT beendet");
#endif
      Liter.setText(cLiter);
    }
  }
  mqtt.disconnect();
}


void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
#ifdef DEBUG_MQTT
  Serial.print("Verbinde zu MQTT... ");
#endif
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
#ifdef DEBUG_MQTT
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    Serial.print("retries: ");
    Serial.println(retries);
#endif
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
#ifdef DEBUG_MQTT
      Serial.println("MQTT Verbindung nach 3 Versuchen fehlgeschlagen");
#endif
      // basically die and wait for WDT to reset me
      // while (1);
      break;
    }
  }

  // publish to {username}/feeds/onoff/get to trigger the last value resend
  // once the connection is established or restored

#ifdef DEBUG_MQTT
  Serial.println("publishing to /get");
#endif
  Zisterne_Liter_get.publish((uint32_t)0);
}
