

#include <SoftwareSerial.h>

#define SDWRITE
//#define DEBUG
#define MAX_DELAY 100 //write on the sdcard every 10 second

#ifdef SDWRITE
  #include <SD.h> 
#endif
//SDCARD SETUP
const int chipSelect = 4;
String FILENAME = "data1.txt"; //Default filename

boolean gps_ready = false;
SoftwareSerial gpsSerial(7,8); // RX, TX

boolean GPS_ENABLE = true;
boolean BT_ENABLE = false;

int pinBT = 5;
int pinLEDRed = 6;
int pinLEDBlue = 9;
int pinLEDGreen = 10;

unsigned long lastPressedMode = 0;
unsigned long lastPressedGPS = 0;
unsigned long lastWrite = 0;

boolean isFirstWrite = true;


struct GNRMC
{
   float lastFix;
   bool status;
   char lat[20];
   char latD;
   char lng[20];
   char lngD;
   unsigned long date;
   

};

struct GNGSA
{
   uint8_t statusFix;
   uint8_t numSat;   
};

GNRMC msg_GNRMC;
GNGSA msg_GNGSA;

void setup() {
  
  Serial.begin(9600);
  gpsSerial.begin(9600);  
  
  pinMode(pinBT,OUTPUT);
  pinMode(pinLEDRed,OUTPUT);
  pinMode(pinLEDBlue,OUTPUT);
  pinMode(pinLEDGreen,OUTPUT);
  digitalWrite(pinBT,HIGH);
  
  attachInterrupt(0, buttonPower, RISING);
  attachInterrupt(1, buttonMode, RISING);

  ledColor(0,0,0);
  
  #ifdef SDWRITE
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(chipSelect, OUTPUT);  
  if (!SD.begin(chipSelect)) {
    ledColor(25,0,0); //Solid led means something wrong
    #ifdef DEBUG
    gpsSerial.println("Card failed, or not present");
    #endif
    // don't do anything more:
    while(true);

  }
  #ifdef DEBUG
  gpsSerial.println("card initialized.");
  #endif
  createNewFile();
  
  #endif
  
  //Blinking means ok
  ledColor(0,25,0);
  delay(100);
  ledColor(0,0,0);
  delay(100);
  ledColor(0,25,0);
  delay(100);
  ledColor(0,0,0);
  delay(1000);
  
//  setNavigation();
  
}
  
  


void loop() {
  
#ifdef DEBUG 
   //if (gps_ready) {
   /* gpsSerial.print(F("Location: ")); 
     if (gps.location.isValid()) {
         gpsSerial.print(gps.location.lat(), 6);
         gpsSerial.print(F(","));
         gpsSerial.print(gps.location.lng(), 6);
     } else 
         gpsSerial.print(F("INVALID"));
     gpsSerial.println();*/
   //}
#endif


#ifdef SDWRITE   
 char tmpFile[FILENAME.length()+1];
 FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
 File dataFile = SD.open(tmpFile,  O_CREAT | O_APPEND | O_WRITE); //Append the file 
 //Make sure that file is ready and gps as well
 if (dataFile) {
    if (GPS_ENABLE) {
   
    if (gps_ready) {
     // if ((millis()-lastWrite) > MAX_DELAY) {
              // #ifdef DEBUG
                gpsSerial.println("Location wrote into the file");
              //  #endif
              //Write coma to separate records
              if (!isFirstWrite) dataFile.print(",");
              else isFirstWrite = false;
              //Write data
	      dataFile.print("{\"lat\":\"");
	      dataFile.print(msg_GNRMC.lat);
	      dataFile.print("\", \"lng\":\"");
	      dataFile.print(msg_GNRMC.lng);
	      dataFile.print("\", \"sat\":\"");
	      dataFile.print(msg_GNGSA.numSat,6);
	      
	      dataFile.print("\",\"time\": ");
	 
              dataFile.print(msg_GNRMC.date);
              dataFile.println("}");
              
	      lastWrite = millis();
    //  }
    }
    }
    dataFile.close();
 }
#endif
  
 
 //Read command from bluetooth and dont read update from GPS
 if (BT_ENABLE) {
     readCommand();
     ledColor(0,0,25);
 }
 else
     if (GPS_ENABLE) {
         readGPS();
         if (gps_ready) //CHANGE THIS //  if (gps.location.isValid() && gps.location.isUpdated())
           ledColor(0,25,0);
         else
           ledColor(25,0,50);
     } else { //Led off, that means not action made
          ledColor(0,0,0);
     }
}


// Enable or disable the gps tracking
void buttonPower() {
  if ((millis()-lastPressedGPS) < 1000) return;
  #ifdef DEBUG
  gpsSerial.println("button mode");
  #endif
  GPS_ENABLE = !GPS_ENABLE;
  gpsSerial.print("Mode gps is: "); gpsSerial.println(GPS_ENABLE);
  lastPressedGPS = millis();
    #ifdef DEBUG
   gpsSerial.println("button power pressed");
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
   gpsSerial.println("button mode pressed");
   #endif
}


void ledColor(int red,int green,int blue) {
  
  analogWrite(pinLEDRed,255);
  analogWrite(pinLEDGreen,255);
  analogWrite(pinLEDBlue,255);
}
