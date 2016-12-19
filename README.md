# Arduino_vBus

I was working to Build up a System, which shows me Time, Temperature and Solar/Heating Datas.

- Arduino Due
- Arduino WiFi Shield
- Adafruit BME280 Humidity + Barometric Pressure + Temperature Sensor Breakout over IC2
- Nextion Enhanced 7" Display NX8048K070_011R 

Time:
Nextion Enhanced is running with an RTC Clock. The Clock will be synced with NTP.

Temperature:
Temperature is pushed over IC2 from the BME280 to the Arduino

Resol vBus :
The Communication is established over HTTP with the Resol Datenlogger DL2. The Datas are convertet from a JSON Table.

Display:
The Display has an own HMI. It gots only the Signals over UART from the Arduino DUE (Serial2).
