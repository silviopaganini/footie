#include <SoftwareSerial.h>

SoftwareSerial gps(7,8); // RX, TX

int pinBT = 5;

void setup() {
  // start serial port at 9600 bps

  pinMode(pinBT,OUTPUT);
    digitalWrite(pinBT,HIGH);
  Serial.begin(9600);
   gps.begin(9600);  
  

}

void loop() {

   //gps.println(Serial.available());
   if (Serial.available() > 0) {
    // get incoming byte:
    gps.write(Serial.read());
  }
 

}

