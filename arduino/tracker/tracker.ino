#include <TinyGPS.h>
#include <SD.h>


TinyGPS gps;

const int chipSelect = 4;


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
    if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}
  
  


void loop() {
  float flat, flon;
  unsigned long age = 0;
  
  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  Serial.println();
  
   File dataFile = SD.open("datalog.txt", FILE_WRITE);
   
    if (dataFile) {
    dataFile.println(flat);
    dataFile.close();
    }
  
  smartdelay(1000);
}

