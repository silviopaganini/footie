#include <SoftwareSerial.h>

SoftwareSerial gps(7,8); // RX, TX

int pinBT = 5;

 uint8_t setNav[] = {
 0xB5, 0X62, 0X06, 0x24, 0x00, 0x00, 0x2A, 0x84 };

void setup() {
  // start serial port at 9600 bps
  Serial.begin(9600);
  pinMode(pinBT,OUTPUT);
  digitalWrite(pinBT,HIGH);
  gps.begin(9600); 
 
  delay(1000); 
  
 // sendCommand();

}

void sendCommand() {
   for(int i=0; i<8; i++) {
   Serial.write(setNav[i]);
   }
   Serial.println();
   
}

void loop() {

  if (Serial.available() > 0) {
      gps.write(Serial.read());
  }
  

}

