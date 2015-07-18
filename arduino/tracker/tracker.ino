#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define SDWRITE
#define DEBUG
#define MAX_DELAY 10000 //write on the sdcard every 10 second

#ifdef SDWRITE
  #include <SD.h> 
#endif
//SDCARD SETUP
const int chipSelect = 4;
String FILENAME = "data1.txt"; //Default filename

TinyGPSPlus gps;
boolean gps_ready = false;
SoftwareSerial gpsSerial(8, 7); // RX, TX

boolean GPS_ENABLE = true;
boolean BT_ENABLE = false;

int pinBT = 5;
int pinLEDRed = 10;
int pinLEDBlue = 6;
int pinLEDGreen = 9;

unsigned long lastPressedMode = 0;
unsigned long lastPressedGPS = 0;
unsigned long lastWrite = 0;


void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);  
  
  pinMode(pinBT,OUTPUT);
  pinMode(pinLEDRed,OUTPUT);
  pinMode(pinLEDBlue,OUTPUT);
  pinMode(pinLEDGreen,OUTPUT);
  ledColor(0,0,0);
  digitalWrite(pinBT,LOW);
  
  attachInterrupt(0, buttonPower, RISING);
  attachInterrupt(1, buttonMode, RISING);

  
  #ifdef SDWRITE
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(chipSelect, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    ledColor(255,0,0); //Solid led means something wrong
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while(true);

  }
  Serial.println("card initialized.");
  createNewFile();
  
  #endif
  
  //Blinking means ok
  ledColor(0,255,0);
  delay(100);
  ledColor(0,0,0);
  delay(100);
  ledColor(0,255,0);
  delay(100);
  ledColor(0,0,0);
  delay(1000);
  
}
  
  


void loop() {
  
#ifdef DEBUG 
   //if (gps_ready) {
   /*  Serial.print(F("Location: ")); 
     if (gps.location.isValid()) {
         Serial.print(gps.location.lat(), 6);
         Serial.print(F(","));
         Serial.print(gps.location.lng(), 6);
     } else 
         Serial.print(F("INVALID"));
     Serial.println();*/
   //}
#endif
#ifdef SDWRITE   
 char tmpFile[FILENAME.length()+1];
 FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
 File dataFile = SD.open(tmpFile,  O_CREAT | O_APPEND | O_WRITE); //Append the file 
 //Make sure that file is ready and gps as well
 if (dataFile) {
   // if (gps_ready) {
   
    if (gps.location.isValid()) {
      if ((millis()-lastWrite) > MAX_DELAY) {
               #ifdef DEBUG
                Serial.println("Location wrote into the file");
                #endif
	      dataFile.print("[lat;");
	      dataFile.print(gps.location.lat(),6);
	      dataFile.print(",lng:");
	      dataFile.print(gps.location.lng(),6);
	      
	      dataFile.print(",time:");
	      if (gps.date.isValid() && gps.time.isValid()) {
		    unsigned long timestamp = makeTime(gps.time.hour(),gps.time.minute(),gps.time.second(),gps.date.day(),gps.date.month(),gps.date.year());
                    dataFile.print(timestamp);
              }
	      else
		 dataFile.print("-1"); 
              dataFile.println("],");
              
	      lastWrite = millis();
      }
    }
   // }
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
         if (gps.location.isValid())
           ledColor(0,255,0);
         else
           ledColor(255,100,50);
     } else { //Led off, that means not action made
          ledColor(0,0,0);
     }
}


// Enable or disable the gps tracking
void buttonPower() {
  if ((millis()-lastPressedGPS) < 1000) return;
  Serial.println("button mode");
  GPS_ENABLE = !GPS_ENABLE;
  lastPressedGPS = millis();
    #ifdef DEBUG
   Serial.println("button power pressed");
   #endif
}

//Change mode Bluetooth or GPS
void buttonMode() {
  if ((millis()-lastPressedMode) < 1000) return;
 
  if (BT_ENABLE) {
      BT_ENABLE = false;
      GPS_ENABLE  = true;  
      digitalWrite(pinBT,LOW);
  } else {
      BT_ENABLE = true;
      GPS_ENABLE  = false;
      digitalWrite(pinBT,HIGH);
  }
  lastPressedMode = millis();
  #ifdef DEBUG
   Serial.println("button mode pressed");
   #endif
}


void ledColor(int red,int green,int blue) {
  analogWrite(pinLEDRed,red);
  analogWrite(pinLEDGreen,green);
  analogWrite(pinLEDBlue,blue);
}
