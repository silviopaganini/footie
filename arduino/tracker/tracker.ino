#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define SDWRITE
//#define DEBUG

#ifdef SDWRITE
  #include <SD.h> 
#endif
//SDCARD SETUP
const int chipSelect = 4;
String FILENAME = "data1.txt"; //Default filename

TinyGPSPlus gps;
boolean gps_ready = false;
SoftwareSerial gpsSerial(2, 3); // RX, TX

boolean GPS_ENABLE = true;
boolean BT_ENABLE = false;

int pinBT = 5;
int pinLEDRed = 6;
int pinLEDBlue = 6;
int pinLEDGreen = 6;


void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);  
  
  pinMode(pinBT,INPUT);
  pinMode(pinLEDRed,OUTPUT);
  pinMode(pinLEDBlue,OUTPUT);
  pinMode(pinLEDGreen,OUTPUT);
  
  attachInterrupt(0, buttonPower, CHANGE);
  attachInterrupt(1, buttonMode, CHANGE);

  
  #ifdef SDWRITE
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(chipSelect, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    ledColor(255,0,0); //Solid led means something wrong
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  createNewFile();
  
  #endif
  
  //Blinking means ok
  ledColor(0,255,0);
  delay(10);
  ledColor(0,0,0);
  delay(10);
  ledColor(0,255,0);
  delay(10);
  ledColor(0,0,0);
  
}
  
  


void loop() {
  
#ifdef DEBUG 
   if (gps_ready) {
     Serial.print(F("Location: ")); 
     if (gps.location.isValid()) {
         Serial.print(gps.location.lat(), 6);
         Serial.print(F(","));
         Serial.print(gps.location.lng(), 6);
     } else 
         Serial.print(F("INVALID"));
     Serial.println();
   }
#endif
#ifdef SDWRITE   
 char tmpFile[FILENAME.length()+1];
 FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
 File dataFile = SD.open(tmpFile,  O_CREAT | O_APPEND | O_WRITE); //Append the file 
 //Make sure that file is ready and gps as well
 if (dataFile) {
    if (gps_ready) {
    #ifdef DEBUG
    Serial.println("Location wrote into the file");
    #endif
    if (gps.location.isValid()) {
      dataFile.print("la:");
      dataFile.print(gps.location.lat(),6);
      dataFile.print("lg:");
      dataFile.print(gps.location.lng(),6);
      
      dataFile.print("d:");
      if (gps.date.isValid()) 
          dataFile.print(gps.date.value());
      else 
         dataFile.print("-1"); 
          
      dataFile.print("t:");
      if (gps.time.isValid())
         dataFile.println(gps.time.value());
      else
         dataFile.println("-1");
    }
    }
    dataFile.close();
 }
#endif
  
 
 //Read command from bluetooth and dont read update from GPS
 if (BT_ENABLE) {
     readCommand();
     ledColor(0,0,255);
 }
 else
     if (GPS_ENABLE) {
         readGPS();
         ledColor(0,255,0);
     } else { //Led off, that means not action made
          ledColor(0,0,0);
     }
}


// Enable or disable the gps tracking
void buttonPower() {
  GPS_ENABLE = !GPS_ENABLE;
}

//Change mode Bluetooth or GPS
void buttonMode() {
  if (BT_ENABLE) {
      BT_ENABLE = false;
      GPS_ENABLE  = true;
  } else {
      BT_ENABLE = true;
      GPS_ENABLE  = false;
  }
}


void ledColor(int red,int green,int blue) {
  analogWrite(pinLEDRed,red);
  analogWrite(pinLEDGreen,green);
  analogWrite(pinLEDBlue,blue);
}
