# Arduino_vBus

<img style="border-width:0" src="https://cloud.githubusercontent.com/assets/15777227/21324662/05cc2a82-c623-11e6-8fe8-fc3cb75cf664.png">


I was working to Build up a System, which shows me Time, Temperature and Solar/Heating Datas.

**Hardware:**
- Arduino Due
- Arduino WiFi Shield
- Adafruit BME280 Humidity + Barometric Pressure + Temperature Sensor Breakout over IC2
- Nextion Enhanced 7" Display NX8048K070_011R 
- Resol DL2

**Function Time:**
Nextion Enhanced is running with an RTC Clock. The Clock will be synced with NTP at startup.

**Function Temperature:**
Temperature is pushed over IC2 every 30 seconds from the BME280 to the Arduino

**Function Resol vBus :**
The Communication is established over HTTP with the Resol Datenlogger DL2. The Datas are convertet from a JSON Table. It will be synced every 60 secons

**Function Display:**
The Display has an own HMI. It gots only the Signals over UART from the Arduino DUE (Serial2).

**Libarys:**
- ```#include <SPI.h>```
- ```#include <Wire.h>```
- ```#include <WiFi.h>```
- ```#include <WiFiUdp.h>```
- ```#include <avr/dtostrf.h>```
- ```#include <stdlib.h>```
- ```#include <TimeLib.h>``` https://github.com/PaulStoffregen/Time
- ```#include <ArduinoJson.h>``` https://github.com/bblanchon/ArduinoJson
- ```#include <Adafruit_Sensor.h>``` https://github.com/adafruit/Adafruit_Sensor
- ```#include <Adafruit_BME280.h>``` https://github.com/adafruit/Adafruit_BME280_Library
- ```#include "Nextion.h"``` https://github.com/itead/ITEADLIB_Arduino_Nextion

The Libary ITEADLIB_Arduino_Nextion needs the latest Pull request (29) to Update the RTC-Commands.
On File "NexUpload.cpp" of this Libary you have to disable line 17 ```(//#include <SoftwareSerial.h>)```
