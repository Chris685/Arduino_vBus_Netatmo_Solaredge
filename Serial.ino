// Initialize Serial port

void initSerial() {
#ifdef DEBUG_SERIAL_ENABLE
  Serial.begin(115200);       // DEBUG_SERIAL_ENABLE Kommikation
  while (!Serial) {
  }
  Serial.println("Serielle Schnittstelle initialisiert");
  Serial.println("");
#endif
}
