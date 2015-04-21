#include <TinyGPS.h>
#include <SoftwareSerial.h>

#define HAS_GPS 13
#define GPS_RX_PIN 4
#define GPS_TX_PIN 3

TinyGPS        gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

void setup()
{
  Serial.begin(9600);
  ss.begin(4800);
  pinMode(HAS_GPS, OUTPUT);

}
void loop()
{
  while (ss.available()) {
    digitalWrite(HAS_GPS, HIGH);
    int c = ss.read();

    gps.encode(c);

    long lat, lon;
    unsigned long fix_age;
    gps.get_position(&lat, &lon, &fix_age);
    if (fix_age == TinyGPS::GPS_INVALID_AGE )
      Serial.println("No fix ever detected!");
    else if (fix_age > 2000)
      Serial.println("Data is getting STALE!");
    else
      Serial.println("Latitude and longitude valid!");
  }
}
