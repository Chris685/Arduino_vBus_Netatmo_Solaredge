#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Nextion.h"
#include <avr/dtostrf.h>
#include <stdlib.h>

// Konfiguration Scheduler
unsigned long ms15000 = 15000;          // 30 Sekunden Takt
unsigned long msold15000;
unsigned long ms30000 = 30000;          // 60 Sekunden Takt
unsigned long msold30000;

// Konfiguration Temperatursensor
Adafruit_BME280 bme;                    // Verbindung ueber I2C

// Konfiguration NTP Zeit
static const char ntpServerName[] = "0.de.pool.ntp.org";
const int timeZone = 1;                 // Central European Time
unsigned int localPort = 8888;          // local port to listen for UDP packets
time_t getNtpTime();
int NTP_sucess;

// Konfiguration WiFi
WiFiUDP Udp;
WiFiClient client;
int status = WL_IDLE_STATUS;

// Konfiguration JSON
char cNexadresse [40];                  // Server Adresse (kommt von Nextion Display)
const char* resource = "dlx/download/live?sessionAuthUsername=admin&sessionAuthPassword=";    //Server Adresse                  
const unsigned long HTTP_TIMEOUT = 6000;  // Maximale Antwortzeit Server
const size_t MAX_CONTENT_SIZE = 9000;     // max size of the HTTP response

// Konfiguration JSON Struct zum decodieren
struct vBus {
  char Kollektor[5];
  char Waermetauscher[5];
  char Speicher_unten [5];
  char Speicher_oben [5];
  char Speicher_mitte [5];
  char Festbrennstoff [5];
  char Pumpe_Festbrennstoff [5];
  char Pumpe_primaer [5];
  char Pumpe_sekundaer [5];
  char Zeit [5];
};


// Konfiguration Deklariere Nextion
NexRtc  rtc;                              // RealTimeClock
NexText Temp = NexText(0, 3, "Temp");
NexText Humity = NexText(0, 3, "Humity");
NexText Druck = NexText(0, 3, "Druck");
NexText speicher_oben = NexText(0, 3, "speicher_oben");
NexText speicher_mitte = NexText(0, 3, "speicher_mitte");
NexText speicher_unten = NexText(0, 3, "speicher_unten");
NexText festbrennstoff = NexText(0, 3, "festbrennstoff");
NexText kollektor = NexText(0, 3, "kollektor");
NexPicture Background = NexPicture(0, 3, "Startseite");
NexVariable NexSSID = NexVariable(0, 3, "SSID");
NexVariable Nexpass = NexVariable(0, 3, "pass");
NexVariable Nexadresse = NexVariable (0, 3, "adresse");
NexPicture p0 = NexPicture(0, 1, "p0");

void setup() {
  initSerial();       // Start Serielle Schnittstelle
  nexInit();          // Start Nextion Display
  initBME280();       // Start Temperatursensor
  readBME280();       // einmaliges lesen Temperatursensor
  initJSON();         // Start JSON - holen der Adresse von Nextion Display
  initWifi();         // Start WiFi
  UpdateTime();       // einmaliges synchronisieren der RTC von Nexton Display
  readJSON();         // einmaliges lesen vBUS Daten
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - msold15000 >= ms15000) {      //15 Sekunden Schleife
    msold15000 = currentMillis;
    Serial.println("15 Sekunden slot");
     readBME280();    // lesen Temperatursensor
    }
   if ((currentMillis - msold30000 >= ms30000)) {   //30 Sekunden Schleife
     msold30000 = currentMillis;
      Serial.println("30 Sekunden slot");
      readJSON();     // lesen vBUS Daten
   }
}


