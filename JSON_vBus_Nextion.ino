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


unsigned long ms30000 = 30000;          // 30 Sekunden Takt
unsigned long msold30000;
unsigned long ms60000 = 60000;          // 60 Sekunden Takt
unsigned long msold60000;

Adafruit_BME280 bme; // I2C

char ssid[] = "";     //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA)
static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = 1;     // Central European Time
unsigned int localPort = 8888;  // local port to listen for UDP packets
WiFiUDP Udp;
WiFiClient client;
int status = WL_IDLE_STATUS;
time_t getNtpTime();
int NTP_sucess;

const char* server = "192.168.1.21";       // Server Adresse
const char* resource = "dlx/download/live?sessionAuthUsername=admin&sessionAuthPassword=";    //Server Adresse                  
const unsigned long HTTP_TIMEOUT = 5000;  // Maximale Antwortzeit Server
const size_t MAX_CONTENT_SIZE = 8512;     // max size of the HTTP response

// JSON Struct zum decodieren
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


//Deklariere Nextion
NexRtc  rtc;
NexText Temp = NexText(0, 3, "Temp");
NexText Humity = NexText(0, 3, "Humity");
NexText Druck = NexText(0, 3, "Druck");
NexText speicher_oben = NexText(0, 3, "speicher_oben");
NexText speicher_mitte = NexText(0, 3, "speicher_mitte");
NexText speicher_unten = NexText(0, 3, "speicher_unten");
NexText festbrennstoff = NexText(0, 3, "festbrennstoff");
NexText kollektor = NexText(0, 3, "kollektor");
NexText JSONZeit = NexText(1, 3, "JSONZeit");
NexPicture Background = NexPicture(0, 3, "Startseite");
//NexNumber rtc3 = NexNumber(0, 3, "rtc3");
//NexNumber rtc4 = NexNumber(0, 3, "rtc4");

void setup() {
  initSerial();
  nexInit();
  initBME280();
  readBME280();
  initWifi();
  UpdateTime();
  readJSON();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - msold30000 >= ms30000) {
    msold30000 = currentMillis;
     readBME280();
    }
   if ((currentMillis - msold60000 >= ms60000)) {
     msold60000 = currentMillis;
      readJSON();
   }
}


