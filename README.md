# Arduino_vBus_Netatmo_Solaredge

<img style="border-width:0" src="https://user-images.githubusercontent.com/15777227/107142733-32b57980-6931-11eb-853c-b2e8bfcce14c.png">
<img style="border-width:0" src="https://user-images.githubusercontent.com/15777227/107142735-347f3d00-6931-11eb-82f1-3336d8f579df.png">
<img style="border-width:0" src="https://user-images.githubusercontent.com/15777227/107142736-35b06a00-6931-11eb-9c6c-bc5442bb9783.png">


I was working to Build up a System, which shows me Time, Temperature and Solar/Heating Datas.

**Hardware:**
- ESP8266
- Adafruit BME280 Humidity + Barometric Pressure + Temperature Sensor Breakout over IC2
- Nextion Enhanced 7" Display NX8048K070_011R 
- Resol DL2
- Solaredge with enabled ModbusTCP
- Netatmo

**Function NTP:**
Nextion Enhanced is running with an RTC Clock. The Clock will be synced with NTP at startup.

**Function Temperature / BME280:**
Temperature is pushed over IC2 every 30 seconds from the BME280 to the Arduino

**Function Resol vBus :**
The Communication is established over HTTP with the Resol Datenlogger DL2. The Datas are convertet from a JSON Table. It will be synced every 60 secons

**Function Display:**
The Display has an own HMI. It gots only the Signals over UART from the Arduino DUE (Serial2).

**Function OTA:**
The Controller can now be Updated over the Air. Just use Webbrowser to Upload Binary Sketch.

**Function SolarEdge:**
Getting Datas from Solaregdge Inverter to visualize current Power and Consumption.

**Function MQTT:**
Getting Datas from a MQTT Server. I use that function to read in our actuall available water at the storage.

**Function NETATMO:**
Getting Datas from Netatmo Weather Station.

**Libarys:**
- ```#include <FS.h>```
- ```#include <LittleFS.h>```
- ```#include <ESP8266HTTPClient.h>```
- ```#include <WiFiClientSecureBearSSL.h>```
- ```#include <ESP8266WebServer.h>```
- ```#include <ESP8266mDNS.h>```
- ```#include <ESP8266HTTPUpdateServer.h>```
- ```#include <ModbusIP_ESP8266.h>```
- ```#include <Arduino.h>```
- ```#include "Adafruit_MQTT.h"```
- ```#include "Adafruit_MQTT_Client.h"```
- ```#include <simpleDSTadjust.h>```
- ```#include <WiFiClientSecure.h>```
- ```#include <time.h>```
- ```#include <ArduinoJson.h>``` https://github.com/bblanchon/ArduinoJson
- ```#include <Adafruit_Sensor.h>``` https://github.com/adafruit/Adafruit_Sensor
- ```#include <Adafruit_BME280.h>``` https://github.com/adafruit/Adafruit_BME280_Library
- ```#include "Nextion.h"``` https://github.com/itead/ITEADLIB_Arduino_Nextion

The Libary ITEADLIB_Arduino_Nextion needs the latest Pull request (29) to Update the RTC-Commands.
On File "NexUpload.cpp" of this Libary you have to disable line 17 ```(//#include <SoftwareSerial.h>)```
