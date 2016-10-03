

#include <SoftwareSerial.h>

#define SDWRITE
//#define DEBUG
#define MAX_DELAY 100 //write on the sdcard every 10 second
//#define PRINTRAM

#ifdef SDWRITE
#include <SD.h> 
#endif
//SDCARD SETUP
const int chipSelect = 4;
String FILENAME = "data1.txt"; //Default filename

boolean GPS_READY = false;
SoftwareSerial gpsSerial(7,8); // RX, TX

boolean GPS_ENABLE = true;
boolean BT_ENABLE = false;

int pinBT = 5;
int pinLEDRed = 6;
int pinLEDBlue = 10;
int pinLEDGreen = 9;

unsigned long lastPressedMode = 0;
unsigned long lastPressedGPS = 0;
unsigned long lastWrite = 0;

boolean isFirstWrite = true;



struct GNRMC
{
  char lastFix[10];
  bool status;
  char latitude[10];
  char latD;
  char longitude[10];
  char lngD;
  char date[10];
  bool written;


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

  attachInterrupt(1, buttonPower, RISING);
  attachInterrupt(0, buttonMode, RISING);

  ledColor(0,0,0);

#ifdef SDWRITE
  initSD();
  createNewFile();
#endif

  //Blinking means ok
  ledColor(0,255,255);
  delay(100);
  ledColor(255,255,255);
  delay(100);
  ledColor(0,255,255);
  delay(100);
  ledColor(255,255,255);
  delay(1000);

  //  setNavigation();
  initStruct();
  updateLed();
  
  
  delay(3000);
  gpsSerial.println("hello");
  setNavigation();
 // sendToGPS();
    gpsSerial.println("done");

}

void loop() {

  readGPS();

#ifdef SDWRITE   
  writeOnFile();
#endif
  //Read command from bluetooth and dont read update from GPS
  if (BT_ENABLE) 
    readCommand();
  else {
    if (GPS_ENABLE) {
      readGPS();
    }
  }
  
  #ifdef PRINTRAM
  gpsSerial.print("Free memory: ");
  gpsSerial.println(freeRam());
  #endif

}

//Init the struct
void initStruct() {
  msg_GNRMC.status = false;
  msg_GNRMC.written = false;
  msg_GNGSA.statusFix = 1;
  msg_GNGSA.numSat = 0;
}



// Enable or disable the gps tracking
void buttonPower() {
  if ((millis()-lastPressedGPS) < 1000) return;

  GPS_ENABLE = !GPS_ENABLE;
  lastPressedGPS = millis();

  updateLed();

#ifdef DEBUG
  gpsSerial.print("Mode gps is: "); 
  gpsSerial.println(GPS_ENABLE);
#endif

}

//Change mode Bluetooth or GPS
void buttonMode() {
  if ((millis()-lastPressedMode) < 1000) return;

  if (BT_ENABLE) {
    BT_ENABLE = false;
    GPS_ENABLE  = true;  
    digitalWrite(pinBT,LOW);
    ledColor(0,255,0);
  } 
  else {
    BT_ENABLE = true;
    GPS_ENABLE  = false;
    digitalWrite(pinBT,HIGH);

  }
  updateLed();
  lastPressedMode = millis();
#ifdef DEBUG
  gpsSerial.println("button mode pressed");
#endif
}




//Method to write the sdcard
#ifdef SDWRITE
void writeOnFile() {

  char tmpFile[FILENAME.length()+1];
  FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
  File dataFile = SD.open(tmpFile,  O_CREAT | O_APPEND | O_WRITE); //Append the file 
  //Make sure that file is ready and gps as well
  if (dataFile) {
    //Is GPS tracking mode enable
    if (GPS_ENABLE) {
      //Is the GPS data ready
      if (GPS_READY && !msg_GNRMC.written) {
        // if ((millis()-lastWrite) > MAX_DELAY) {
        // #ifdef DEBUG
        gpsSerial.println("Location wrote into the file");
        //  #endif
        //Write coma to separate records
        if (!isFirstWrite) dataFile.print(",");
        else isFirstWrite = false;
        //Write data
        dataFile.print("{\"time\":\"");
        dataFile.print(msg_GNRMC.lastFix);
        dataFile.print("\", \"lat\":\"");
        dataFile.print(msg_GNRMC.latitude);
        dataFile.print("\", \"lng\":\"");
        dataFile.print(msg_GNRMC.longitude);
        dataFile.print("\", \"sat\":\"");
        dataFile.print(msg_GNGSA.numSat);

        dataFile.print("\",\"date\": ");

        dataFile.print(msg_GNRMC.date);
        dataFile.println("}");

        lastWrite = millis();
        msg_GNRMC.written = true;
        //  }
      }
    }
    dataFile.close();
  }
}
#endif




/// LED functions

void ledColor(int red,int green,int blue) {
  analogWrite(pinLEDRed,red);
  analogWrite(pinLEDGreen,green);
  analogWrite(pinLEDBlue,blue);
}

void updateLed() {
  #ifdef DEBUG
  gpsSerial.print("GPS ENABLE: ");
  gpsSerial.print(GPS_ENABLE);
  gpsSerial.print(" GPS READY: ");
  gpsSerial.print(GPS_READY);
  gpsSerial.print(" BT ENABLE: ");
  gpsSerial.println(BT_ENABLE);
  #endif
  if (GPS_ENABLE) {
    if (GPS_READY) 
      ledColor(255,0,255); //Green
    else
      ledColor(0,255,0); //-
  }  
  else if (BT_ENABLE) {
    ledColor(255,255,0); //Blue
  } 
  else {
    ledColor(255,255,255); //None
  }
}


//Calculate free ram
#ifdef PRINTRAM
int freeRam () {
 extern int __heap_start, *__brkval; 
 int v; 
 return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
 }
#endif

