void Seitenabfrage () {
  char buffer [20];
  memset(buffer, 0, sizeof(buffer)); //
  Seite.getText(buffer, sizeof(buffer)); //
  N_Seite = atof (buffer);

#ifdef DEBUG_SERIAL_ENABLE
  Serial.print("Nextion Seite: ");
  Serial.println(N_Seite);
#endif
}
