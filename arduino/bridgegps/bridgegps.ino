#include <SoftwareSerial.h>

SoftwareSerial gps(8, 7); // RX, TX


void setup() {
  // start serial port at 9600 bps:
  Serial.begin(9600);
   gps.begin(9600);  
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    gps.write(Serial.read());
    
  }
   if (gps.available() > 0) {
    // get incoming byte:
    Serial.write(gps.read());
    
  }
}

