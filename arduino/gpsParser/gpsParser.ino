#include <SoftwareSerial.h>

SoftwareSerial gps(7,8); // RX, TX

int pinBT = 5;

char buffer[200];
int bufferIndex = 0;
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
    readGPS();
  }
 

}


void readGPS() {
  
  int inChar = Serial.read();
  
  if (inChar == '$') {
     // if (bufferIndex != 0) printBuffer();
     resetBuffer();
  } else {
      if (inChar == '\n') { 
     //  printBuffer();
        createStructure();
        resetBuffer();
      } else {
        buffer[bufferIndex] = (char)inChar;
        bufferIndex++;
      }
  }
}

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
/*
$GNRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *

$GNGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:
     GSA      Satellite status
     A        Auto selection of 2D or 3D fix (M = manual) 
     3        3D fix - values include: 1 = no fix
                                       2 = 2D fix
                                       3 = 3D fix
     04,05... PRNs of satellites used for fix (space for 12) 
     2.5      PDOP (dilution of precision) 
     1.3      Horizontal dilution of precision (HDOP) 
     2.1      Vertical dilution of precision (VDOP)
     *39      the checksum data, always begins with *

*/
void createStructure() {
    if (buffer[0] == 'G') {
       if (buffer[1] == 'N') {
          // GNRMC
           if (buffer[2] == 'R') {
                if (buffer[3] == 'M') {
                     if (buffer[4] == 'C') {
                                       

                      //  gps.println("found GNRMC");
                        parseGNRMC();
                     }
                }
           }
           // GNGSA
           if (buffer[2] == 'G') {
                if (buffer[3] == 'S') {
                     if (buffer[4] == 'A') {
                        printBuffer();
                    //    gps.println("found GNGSA");
                        parseGNGSA();
                     }
                }
           }
           
       }
    }

}



void parseGNRMC() {
    int coma = 0;
    int x = 0;
    char sbuffer[10];
    for (int i = 5;i < bufferIndex;i++) {
       if (buffer[i] == ',') {
         
            // gps.println(coma);
            // gps.println(sbuffer);
          if (coma == 1) {
             gps.println(sbuffer);
             msg_GNRMC.lastFix = atof(sbuffer);
          } else if (coma == 2) {
             if (sbuffer[0] == 'A') msg_GNRMC.status = true;
             else msg_GNRMC.status = false;
          } else if (coma == 3) {               
             strcpy(msg_GNRMC.lat, sbuffer);
          }else if (coma == 4) {               
             msg_GNRMC.latD =  sbuffer[0];
          } else if (coma == 5) {
              strcpy(msg_GNRMC.lng, sbuffer);
          }else if (coma == 6) {                 
             msg_GNRMC.lngD =  sbuffer[0];
          } else if (coma == 9) {
             msg_GNRMC.date = atol(sbuffer);
          } 
            coma += 1;
          x = 0;
          memset(sbuffer, 0, sizeof(sbuffer));
       }else {
           sbuffer[x] = buffer[i];
           x += 1;
       }
    }
    
           gps.print("Last fix"); gps.println(msg_GNRMC.lastFix);
           gps.print("Last status"); gps.println(msg_GNRMC.status);
           gps.print("Lat");gps.print(msg_GNRMC.latD); gps.println(msg_GNRMC.lat);
           gps.print("lng "); gps.print(msg_GNRMC.lngD); gps.println(msg_GNRMC.lng);
           gps.print("date "); gps.println(msg_GNRMC.date);
}

void parseGNGSA() {
    int coma = 0;
    int x = 0;
    char sbuffer[10];
    int numSat = 0;
    for (int i = 5;i < bufferIndex;i++) {
       if (buffer[i] == ',') {
          if (coma == 2) {
              if (sbuffer[0] == '3') msg_GNGSA.statusFix = 3;
              else if (sbuffer[0] == '2') msg_GNGSA.statusFix = 2;
              else if (sbuffer[0] == '1') msg_GNGSA.statusFix = 1;
              else msg_GNGSA.statusFix = 1;
          } else if (coma >= 3 a&& coma <= 11) 
              if (atoi(sbuffer) > 0) numSat += 1;
          } else if (coma == 12) {
              msg_GNGSA.numSat = atoi(sbuffer);
          }
          coma += 1;
          x = 0;
          memset(sbuffer, 0, sizeof(sbuffer));
       } else {
           sbuffer[x] = buffer[i];
           x += 1;
       }
    }
    
     gps.print("status fix is");
      if (msg_GNGSA.statusFix == 3) gps.println(" 3D FIX");
      gps.print("num of sat"); gps.println(msg_GNGSA.numSat);
}


void printBuffer() {
  for (int i = 0;i < bufferIndex;i++) 
     gps.print(buffer[i]);
  gps.println(); 
}

void resetBuffer() {
  memset(buffer, 0, sizeof(buffer));
  bufferIndex = 0;
}


