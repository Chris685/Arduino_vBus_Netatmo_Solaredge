#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ModbusIP_ESP8266.h>
#include <Arduino.h>
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Nextion.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <simpleDSTadjust.h>
#include <time.h>
#include <WiFiClientSecure.h>

#define DEBUG_LED
#define DEBUG_SERIAL_ENABLE
#define DEBUG_NETATMO
//#define DEBUG_vBUS
//#define DEBUG_BME280
//#define DEBUG_NTP
//#define DEBUG_WIFI
//#define DEBUG_OTA
//#define DEBUG_MQTT
//#define DEBUG_SOLAREDGE


// Konfiguration Scheduler
unsigned long ms1000 = 1000;            // 1 Sekunden Takt
unsigned long msold1000;
unsigned long ms15000 = 15000;          // 15 Sekunden Takt
unsigned long msold15000;
unsigned long ms30000 = 30000;          // 30 Sekunden Takt
unsigned long msold30000;
unsigned long ms300000 = 300000;        // 5 Minuten Takt
unsigned long msold300000;
unsigned long ms900000 = 900000;        // 15 Minuten Takt
unsigned long msold900000;
int aktuelle_cTest = 0;

// Konfiguration WiFi
WiFiClient client;

// Konfiguration JSON
char cNexadresse [40];                  // Server Adresse (kommt von Nextion Display)
char resource [] = "/dlx/download/live?sessionAuthUsername=admin&sessionAuthPassword=xxxxxx";    //Server Adresse
const unsigned long HTTP_TIMEOUT = 6000;  // Maximale Antwortzeit Server
const size_t MAX_CONTENT_SIZE = 9000;     // max size of the HTTP response

//Einstellung Modbus MASTER für SolarEDGE Slave
IPAddress remote(192, 168, 1, 26);  // Address of Modbus Slave device
ModbusIP mb;  //ModbusIP object
int status = 0;
uint16_t power_inverter = 0;
uint16_t power_inverter_scale = 0;
uint16_t power_meter = 0;
uint16_t power_meter_scale = 0;
int16_t power_house = 0;

// Adafruit MQTT Setup
#define AIO_SERVER      ""
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Zisterne_Liter = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/zisterne-liter");
Adafruit_MQTT_Publish Zisterne_Liter_get = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/zisterne-liter/get");

// Konfiguration Deklariere Nextion
// RealTimeClock
NexRtc  rtc;
//Einstellungen
NexVariable NexSSID = NexVariable(0, 3, "Startseite.SSID");
NexVariable Nexpass = NexVariable(0, 3, "Startseite.pass");
NexVariable Nexadresse = NexVariable (0, 3, "Startseite.adresse");
NexVariable NexTemperatur = NexVariable (0, 3, "Startseite.Temperatur");
NexVariable NexZeit = NexVariable (0, 3, "Startseite.Zeit");
NexVariable Seite = NexVariable(0, 57, "Startseite.Seite");
unsigned int N_Seite = 0;

//1 Sekunden Alive LED
const int ledred = LED_BUILTIN;
int ledStatered = LOW;
const int ledblue =  2;
int ledStateblue = HIGH;

void setup() {
  initSerial();       // Start Serielle Schnittstelle
  pinMode(ledred, OUTPUT);    // Keep Alive LED
  pinMode(ledblue, OUTPUT);   // Keep Alive LED
#ifndef DEBUG_LED
  digitalWrite(ledred, HIGH);    //Alive LED
  digitalWrite(ledblue, HIGH);    //Alive LED
#endif
  //LittleFS.format();
  if (!LittleFS.begin()) {
#ifdef DEBUG_NETATMO
    Serial.println("Nutzung ESP8266 Datei System fehlgeschlagen");
#endif
    return;
  }
  initBME280();       // Start Temperatursensor
  delay(1000);        // Pause damit Nextion hochfährt
  nexInit();          // Start Nextion Display
  delay(1000);        // Pause Nextion
  readBME280();       // einmaliges lesen Temperatursensor
  initvBUS();         // Start JSON - holen der Adresse von Nextion Display
  initWifi();         // Start WiFi
  Lese_Einstellungen_Netatmo();   //Access Token für Netatmo laden
  mb.client();        // Modbus Init
  readSolarEdge();    // SolarEdge
  initOTA();          // init OTA Update - Webserver
  updateNTP();        // einmaliges synchronisieren der RTC von Nexton Display
  readvBUS();         // einmaliges lesen vBUS Daten
  mqtt.subscribe(&Zisterne_Liter);    //Auswählen der MQTT Daten
  MQTT();             // Abfragen der MQTT Daten
  int ret = readNetatmo();   //Abfrage Daten Netatmo
  readSolarEdge();    // SolarEdge
}

void loop() {
  OTA();
  unsigned long currentMillis = millis();
#ifdef DEBUG_LED
  if (currentMillis - msold1000 >= ms1000) {         //1 Sekunden Schleife - aktuell nur für LED
    msold1000 = currentMillis;
    if (ledStatered == LOW) {
      ledStatered = HIGH;
    }
    else
    {
      ledStatered = LOW;
    }
    if (ledStateblue == HIGH) {
      ledStateblue = LOW;
    }
    else
    {
      ledStateblue = HIGH;
    }
    digitalWrite(ledred, ledStatered);    //Alive LED
    digitalWrite(ledblue, ledStateblue);    //Alive LED
  }
#endif
  if (currentMillis - msold15000 >= ms15000) {         //15 Sekunden Schleife - aktuell BME280, vBUS, SolarEdge
    msold15000 = currentMillis;
    Seitenabfrage ();
    readBME280();      // lesen Temperatursensor
    readvBUS();        // lesen vBUS Daten
    readSolarEdge();   // SolarEdge
  }
  if ((currentMillis - msold30000 >= ms30000)) {   //30 Sekunden Schleife - aktuell nur für DEBUG
    msold30000 = currentMillis;
#ifdef DEBUG_SERIAL_ENABLE
    Serial.print("\nFreeHeap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.print(" HeapFragmentation: ");
    Serial.print(ESP.getHeapFragmentation());
    Serial.print(" MaxFreeBlockSize: ");
    Serial.println(ESP.getMaxFreeBlockSize());
    Serial.println("\n30 Sekunden Schleife");
#endif
  }
  if ((currentMillis - msold300000 >= ms300000)) {   //5 Minuten Schleife
    msold300000 = currentMillis;
#ifdef DEBUG_SERIAL_ENABLE
    Serial.println("");
    Serial.println("5 Minuten Schleife");
#endif
    Seitenabfrage ();
    int ret = readNetatmo();
  }


  if ((currentMillis - msold900000 >= ms900000)) {   //15 Minuten Schleife
    msold900000 = currentMillis;
#ifdef DEBUG_SERIAL_ENABLE
    Serial.println("");
    Serial.println("15 Minuten Schleife");
#endif
    Seitenabfrage ();
    MQTT();
  }
}
