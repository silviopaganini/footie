
char buffer[100];
int bufferIndex = 0;
void readGPS()
{

  while (Serial.available() > 0) {
    int inChar = Serial.read();

    if (inChar == '$') {
      resetBuffer();
    } 
    else {
      if (inChar == '\n' || bufferIndex > 99) { 
       // printBuffer();
        createStructure();
        resetBuffer();
      } 
      else {
        buffer[bufferIndex] = (char)inChar;
        bufferIndex++;
      }
    }
  }


}

/*

 uint8_t setNav[] = {
 0xB5, 0X62, 0X06, 0x02, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0D, 0xAC };
 //   0xB5, 0x62, 0x06 , 0x24 , 0x24 , 0x00 , 0xFF , 0xFF , 0x03 , 0x03 , 0x00 , 0x00 , 0x00 , 0x00 , 0x10, 0x27 , 0x00 , 0x00 , 0x05 , 0x00 , 0xFA , 0x00 , 0xFA , 0x00 , 0x64 , 0x00 , 0x2C , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x10 , 0x27 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x4A , 0x75                       };
 
 byte gps_set_sucess = 0 ;
 
 
 void setNavigation() {
 while(!gps_set_sucess)
 {
 sendUBX(setNav, sizeof(setNav)/sizeof(uint8_t));
 gps_set_sucess=getUBX_ACK(setNav);
 }
 gps_set_sucess=0;
 }
 
 
 // Send a byte array of UBX protocol to the GPS
 void sendUBX(uint8_t *MSG, uint8_t len) {
 for(int i=0; i<len; i++) {
 Serial.write(MSG[i]);
 }
 Serial.println();
 }
 
 
 
 // Calculate expected UBX ACK packet and parse UBX response from GPS
 boolean getUBX_ACK(uint8_t *MSG) {
 uint8_t b;
 uint8_t ackByteID = 0;
 uint8_t ackPacket[10];
 unsigned long startTime = millis();
 Serial.print(" * Reading ACK response: ");
 
 // Construct the expected ACK packet    
 ackPacket[0] = 0xB5;	// header
 ackPacket[1] = 0x62;	// header
 ackPacket[2] = 0x05;	// class
 ackPacket[3] = 0x01;	// id
 ackPacket[4] = 0x02;	// length
 ackPacket[5] = 0x00;
 ackPacket[6] = MSG[2];	// ACK class
 ackPacket[7] = MSG[3];	// ACK id
 ackPacket[8] = 0;		// CK_A
 ackPacket[9] = 0;		// CK_B
 
 // Calculate the checksums
 for (uint8_t i=2; i<8; i++) {
 ackPacket[8] = ackPacket[8] + ackPacket[i];
 ackPacket[9] = ackPacket[9] + ackPacket[8];
 }
 
 while (1) {
 
 // Test for success
 if (ackByteID > 9) {
 // All packets in order!
 Serial.println(" (SUCCESS!)");
 return true;
 }
 
 // Timeout if no valid response in 3 seconds
 if (millis() - startTime > 3000) { 
 Serial.println(" (FAILED!)");
 return false;
 }
 
 // Make sure data is available to read
 if (Serial.available()) {
 b = Serial.read();
 
 // Check that bytes arrive in sequence as per expected ACK packet
 if (b == ackPacket[ackByteID]) { 
 ackByteID++;
 Serial.print(b, HEX);
 } 
 else {
 ackByteID = 0;	// Reset and look again, invalid order
 }
 
 }
 }
 }
 */

/*
$sk,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
 
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
            //   printBuffer();               

            //  gpsSerial.println("found GNRMC");
            parseGNRMC();
          }
        }
      }
      // GNGSA
      if (buffer[2] == 'G') {
        if (buffer[3] == 'S') {
          if (buffer[4] == 'A') {
            //  printBuffer();
            //    gpsSerial.println("found GNGSA");
             
            parseGNGSA();
          }
        }
      }

    }
     //GPGSV
     if (buffer[1] == 'P') {
            if (buffer[2] == 'G') {
                    if (buffer[3] == 'S') {
                            if (buffer[4] == 'V') {
                              parseGPGSV();
                            }
                    }
            }
     }
  }

}


//$GPRMC,hhmmss,status,latitude,N,longitude,E,spd,cog,ddmmyy,mv,mvE,mode*cs<CR><LF>
void parseGNRMC() {
  int coma = 0;
  int x = 0;
  char sbuffer[10];
  for (int i = 5;i < bufferIndex;i++) {
    if (buffer[i] == ',') {
      if (coma == 1) {
         msg_GNRMC.lastFix = atol(sbuffer);
        //strcpy(msg_GNRMC.lastFix, sbuffer);
      } 
      else if (coma == 2) {
        if (sbuffer[0] == 'A') msg_GNRMC.status = true;
        else msg_GNRMC.status = false;
      } 
      else if (coma == 3) {               
        strcpy(msg_GNRMC.latitude, sbuffer);
      }
      else if (coma == 4) {  
        msg_GNRMC.latD =  sbuffer[0];
        //Convert string to float
        char mlat[8] = {msg_GNRMC.latitude[0],msg_GNRMC.latitude[1],msg_GNRMC.latitude[2],msg_GNRMC.latitude[3],msg_GNRMC.latitude[4],msg_GNRMC.latitude[5],msg_GNRMC.latitude[6],msg_GNRMC.latitude[7]}; 
        double  lat = atof(mlat);  
        int head = (int)(lat/100);
        lat =  (lat - (head*100))/60 + head;
        //Convert float to string
        if (msg_GNRMC.latD  == 'N') {
           dtostrf(lat,2,6,msg_GNRMC.latitude);
        } else {
           dtostrf(-lat,2,6,msg_GNRMC.latitude);
        }
      } 
      else if (coma == 5) {
        strcpy(msg_GNRMC.longitude, sbuffer);
      }
      else if (coma == 6) {                 
        msg_GNRMC.lngD =  sbuffer[0];
        //Convert string to float
        char mlat[8] = {msg_GNRMC.longitude[0],msg_GNRMC.longitude[1],msg_GNRMC.longitude[2],msg_GNRMC.longitude[3],msg_GNRMC.longitude[4],msg_GNRMC.longitude[5],msg_GNRMC.longitude[6],msg_GNRMC.longitude[7]}; 
        double  lng = atof(mlat);  
        int head = (int)(lng/1000);
        lng =  (lng - (head*1000))/60 + head;
        //Convert float to string
        if (msg_GNRMC.lngD  == 'E') {
           dtostrf(lng,2,6,msg_GNRMC.longitude);
        } else {
           dtostrf(-lng,2,6,msg_GNRMC.longitude);
        }
      } 
      else if (coma == 9) {
        msg_GNRMC.date = atol(sbuffer);
        //strcpy(msg_GNRMC.date, sbuffer);
      } 
      coma += 1;
      x = 0;
      memset(sbuffer, 0, sizeof(sbuffer));
    }
    else {
      sbuffer[x] = buffer[i];
      x += 1;
    }
  }
  
  if (msg_GNRMC.written) msg_GNRMC.written = false;
  
#ifdef DEBUG
  gpsSerial.print("Last fix"); 
  gpsSerial.println(msg_GNRMC.lastFix);
  gpsSerial.print("Last status"); 
  gpsSerial.println(msg_GNRMC.status);
  gpsSerial.print("Lat");
 // gpsSerial.println(msg_GNRMC.latD); 
  gpsSerial.println(msg_GNRMC.latitude);
  gpsSerial.print("lng "); 
//  gpsSerial.print(msg_GNRMC.lngD); */
  gpsSerial.println(msg_GNRMC.longitude);
  
  gpsSerial.print("date "); 
  gpsSerial.println(msg_GNRMC.date);

#endif
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
      } 
      else if (coma >= 3 && coma <= 11) {
        if (atoi(sbuffer) > 0) numSat += 1;
      } 
      else if (coma == 12) {
        msg_GNGSA.numSat = numSat;
      }
      coma += 1;
      x = 0;
      memset(sbuffer, 0, sizeof(sbuffer));
    } 
    else {
      sbuffer[x] = buffer[i];
      x += 1;
    }
  }

  if (msg_GNGSA.statusFix == 3 || msg_GNGSA.statusFix == 2) GPS_READY = true;
  else if (msg_GNGSA.statusFix == 1) GPS_READY = false;
  
  updateLed();
  

#ifdef DEBUG
  gpsSerial.print("status fix is: ");
  if (msg_GNGSA.statusFix == 3) gpsSerial.println(" 3D FIX");
  if (msg_GNGSA.statusFix == 2) gpsSerial.println(" 2D FIX");
  if (msg_GNGSA.statusFix == 1) gpsSerial.println(" NO FIX");
  gpsSerial.print("Number of satellite: "); 
  gpsSerial.println(msg_GNGSA.numSat);
#endif
}

void parseGPGSV() {
  int coma = 0;
  int x = 0;
  char sbuffer[10];
  int numSat = 0;
  for (int i = 5;i < bufferIndex;i++) {
    if (buffer[i] == ',') {
      if (coma >= 3 && coma <= 11) {
         msg_GNGSA.numSat = atoi(sbuffer);
         break;
      }   
      coma += 1;
      x = 0;
      memset(sbuffer, 0, sizeof(sbuffer));
    } 
    else {
      sbuffer[x] = buffer[i];
      x += 1;
    }
  }


#ifdef DEBUG
  gpsSerial.print("status fix is: ");
  if (msg_GNGSA.statusFix == 3) gpsSerial.println(" 3D FIX");
  if (msg_GNGSA.statusFix == 2) gpsSerial.println(" 2D FIX");
  if (msg_GNGSA.statusFix == 1) gpsSerial.println(" NO FIX");
  gpsSerial.print("Number of satellite: "); 
  gpsSerial.println(msg_GNGSA.numSat);
#endif
}


void printBuffer() {
  for (int i = 0;i < bufferIndex;i++) 
    gpsSerial.print(buffer[i]);
  gpsSerial.println(); 
}

void resetBuffer() {
  memset(buffer, 0, sizeof(buffer));
  bufferIndex = 0;
}


