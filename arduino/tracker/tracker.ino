#include <TinyGPS++.h>

#define SDWRITE
#define DEBUG

#ifdef SDWRITE
  #include <SD.h> 
#endif
//SDCARD SETUP
const int chipSelect = 4;
String FILENAME = "data1.txt"; //Default filename

TinyGPSPlus gps;
boolean gps_ready = false;



void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  
  
  #ifdef SDWRITE
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(chipSelect, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  createNewFile();
  
  #endif
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
  
 readGPS();
}

