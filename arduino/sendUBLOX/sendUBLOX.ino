#include <SoftwareSerial.h>

SoftwareSerial gps(7,8); // RX, TX

int pinBT = 5;

 uint8_t setNav[] = {
 0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4A, 0x75};

void setup() {
  // start serial port at 9600 bps
  Serial.begin(9600);
  pinMode(pinBT,OUTPUT);
  digitalWrite(pinBT,HIGH);
  gps.begin(9600); 
 
  delay(1000); 
  
  sendCommand(sizeof(setNav)/sizeof(uint8_t));

}

void sendCommand(int len) {
   for(int i=0; i<len; i++) {
   Serial.write(setNav[i]);
   }
   Serial.println();
   
}

void loop() {

 //   sendCommand();

  if (Serial.available() > 0) {
      byte r = Serial.read();
      if (r == '\n') gps.println();
      gps.print(Serial.read(),HEX);
  }
  

}

