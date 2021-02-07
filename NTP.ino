//Zeiteinstellung
#define timezone +1 // CEST/CET
struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0}; // Central European Time = UTC/GMT +1 hour
simpleDSTadjust dstAdjusted(StartRule, EndRule);
#define NTP_SERVERS "ptbtime1.ptb.de", "ptbtime2.ptb.de", "ptbtime3.ptb.de"


void updateNTP() {
#ifdef DEBUG_NTP
  Serial.println("Zeitupdate");
#endif
  configTime(timezone * 3600, 0, NTP_SERVERS);
  delay(500);
  while (!time(nullptr)) {
#ifdef DEBUG_NTP
    Serial.print("#");
#endif
    delay(1000);
  }
  printTime(0);
}

void printTime(time_t offset)
{
  char buf[30];
  char *dstAbbrev;
  time_t t = dstAdjusted.time(&dstAbbrev) + offset;
  struct tm *timeinfo = localtime (&t);
  sprintf(buf, "%02d/%02d/%04d %02d:%02d:%02d %s\n", timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, dstAbbrev);
#ifdef DEBUG_NTP
  Serial.print(buf);
#endif
  if (timeinfo->tm_year != 70)
  {
#ifdef DEBUG_NTP
    Serial.println("Update RTC Nextion Uhrzeit");
#endif
    uint32_t  time[7] = {timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec};
    rtc.write_rtc_time(time); //Uhrzeit an Nextion uebertragen
  }
  else
  {
#ifdef DEBUG_NTP
    Serial.println("RTC Nextion Uhrzeit wird nicht geupdatet");
#endif
  }
}
