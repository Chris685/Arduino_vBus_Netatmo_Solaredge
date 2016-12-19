// Initialize Serial port
void initSerial() {
  Serial.begin(9600);
  Serial2.begin(9600);
  while (!Serial) {
  }
  Serial.println("Serielle Schnittstelle initialisiert");
}
