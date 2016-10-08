
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
        saveBuffer();
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


void saveBuffer() {
#ifdef SDWRITE
  char tmpFile[FILENAME.length()+1];
  FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
  File dataFile = SD.open(tmpFile,  O_CREAT | O_APPEND | O_WRITE); //Append the file 
  //Make sure that file is ready and gps as well

  if (dataFile) {

    dataFile.println(buffer);
  }
  dataFile.close();
#endif
}




uint8_t setNav[] = {
  // 0xB5, 0X62, 0X06, 0x02, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0D, 0xAC };
  0xB5, 0x62, 0x06 , 0x24 , 0x24 , 0x00 , 0xFF , 0xFF , 0x03 , 0x03 , 0x00 , 0x00 , 0x00 , 0x00 , 0x10, 0x27 , 0x00 , 0x00 , 0x05 , 0x00 , 0xFA , 0x00 , 0xFA , 0x00 , 0x64 , 0x00 , 0x2C , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x10 , 0x27 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x4A , 0x75                       };
uint8_t setRate[]  = {  
  0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A };
byte gps_set_sucess = 0 ;


void setNavigation() {
  while(!gps_set_sucess)
  {
    sendUBX(setNav, sizeof(setNav)/sizeof(uint8_t));
    gps_set_sucess=getUBX_ACK(setNav);
  }
  gps_set_sucess=0;
  while(!gps_set_sucess)
  {
    sendUBX(setRate, sizeof(setRate)/sizeof(uint8_t));
    gps_set_sucess=getUBX_ACK(setRate);
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
  gpsSerial.print(" * Reading ACK response: ");

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
      gpsSerial.println(" (SUCCESS!)");
      return true;
    }

    // Timeout if no valid response in 3 seconds
    if (millis() - startTime > 3000) { 
      gpsSerial.println(" (FAILED!)");
      return false;
    }

    // Make sure data is available to read
    if (Serial.available()) {
      b = Serial.read();

      // Check that bytes arrive in sequence as per expected ACK packet
      if (b == ackPacket[ackByteID]) { 
        ackByteID++;
        gpsSerial.print(b, HEX);
      } 
      else {
        ackByteID = 0;	// Reset and look again, invalid order
      }

    }
  }
}


/*

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
 
 eg3. $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
 1    = UTC of Position
 2    = Latitude
 3    = N or S
 4    = Longitude
 5    = E or W
 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
 7    = Number of satellites in use [not those in view]
 8    = Horizontal dilution of position
 9    = Antenna altitude above/below mean sea level (geoid)
 10   = Meters  (Antenna height unit)
 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
 mean sea level.  -=geoid is below WGS-84 ellipsoid)
 12   = Meters  (Units of geoidal separation)
 13   = Age in seconds since last update from diff. reference station
 14   = Diff. reference station ID#
 15   = Checksum
 

 */
void createStructure() {
  if (buffer[0] == 'G') {
    if (buffer[1] == 'N') {
      // GNRMC
      if (buffer[2] == 'R') {
        if (buffer[3] == 'M') {
          if (buffer[4] == 'C') {
            parseGNRMC();
          }
        }
      }
      if (buffer[2] == 'G') {
        // GNGSA
        if (buffer[3] == 'S') {
          if (buffer[4] == 'A') {
            parseGNGSA();
          }
        }
        //GNGGA
        if (buffer[3] == 'G') {
          if (buffer[4] == 'A') {
            parseGNGGA();
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
        strcpy(msg_GPS.lastFix, sbuffer);
      } 
      else if (coma == 2) {
        if (sbuffer[0] == 'A') msg_GPS.status = true;
        else msg_GPS.status = false;
      } 
      else if (coma == 3) {               
        strcpy(msg_GPS.latitude, sbuffer);
      }
      else if (coma == 4) {  
        updateLatitude(sbuffer[0]);
      } 
      else if (coma == 5) {
        strcpy(msg_GPS.longitude, sbuffer);
      }
      else if (coma == 6) {           
        updateLongitude(sbuffer[0]);
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

  if (msg_GPS.written) msg_GPS.written = false;

#ifdef DEBUG
  gpsSerial.print("Last fix"); 
  gpsSerial.println(msg_GPS.lastFix);
  gpsSerial.print("Last status"); 
  gpsSerial.println(msg_GPS.status);
  gpsSerial.print("Lat");
  gpsSerial.println(msg_GPS.latitude);
  gpsSerial.print("lng "); 
  gpsSerial.println(msg_GPS.longitude);
#endif
}


//GNGSA message, get the status FIX which can be 1 = No Fix, 2 = 2D fix, 3 = 3D fix
void parseGNGSA() {
  int coma = 0;
  int x = 0;
  char sbuffer[10];
  int numSat = 0;
  for (int i = 5;i < bufferIndex;i++) {
    if (buffer[i] == ',') {
      if (coma == 2) {
        if (sbuffer[0] == '3') msg_GPS.statusFix = 3;
        else if (sbuffer[0] == '2') msg_GPS.statusFix = 2;
        else if (sbuffer[0] == '1') msg_GPS.statusFix = 1;
        else msg_GPS.statusFix = 1;
        break;
      } 
      gpsSerial.println(sbuffer);
      coma += 1;
      x = 0;
      memset(sbuffer, 0, sizeof(sbuffer));
    } 
    else {
      sbuffer[x] = buffer[i];
      x += 1;
    }
  }
  if (msg_GPS.statusFix == 3 || msg_GPS.statusFix == 2) GPS_READY = true;
  else if (msg_GPS.statusFix == 1) GPS_READY = false;
  updateLed();
#ifdef DEBUG
  gpsSerial.print("status fix is: ");
  if (msg_GPS.statusFix == 3) gpsSerial.println(" 3D FIX");
  if (msg_GPS.statusFix == 2) gpsSerial.println(" 2D FIX");
  if (msg_GPS.statusFix == 1) gpsSerial.println(" NO FIX");
  gpsSerial.print("Number of satellite: "); 
  gpsSerial.println(msg_GPS.numSat);
#endif
}

void parseGNGGA() {
  int coma = 0;
  int x = 0;
  char sbuffer[13];
  int numSat = 0;
  for (int i = 5;i < bufferIndex;i++) {
    if (buffer[i] == ',') {
      if (coma == 1) {
        strcpy(msg_GPS.lastFix, sbuffer);
      } 
      else if (coma == 2) { // Latitude          
        strcpy(msg_GPS.latitude, sbuffer);
      } 
      else if (coma == 3) { //Latitude N or S
        updateLatitude(sbuffer[0]);
      } 
      else if (coma == 4) { //longitude   
        strcpy(msg_GPS.longitude, sbuffer);
      } 
      else if (coma == 5) {  //longitude E or W       
        updateLongitude(sbuffer[0]);  
      } 
      else if (coma == 7) {  //Get amount of satellite used            
        msg_GPS.numSat =  atoi(sbuffer);
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
  gpsSerial.print("Last fix"); 
  gpsSerial.println(msg_GPS.lastFix);
  gpsSerial.print("Last status"); 
  gpsSerial.println(msg_GPS.status);
  gpsSerial.print("Lat");
  gpsSerial.println(msg_GPS.latitude);
  gpsSerial.print("lng "); 
  gpsSerial.println(msg_GPS.longitude);
  gpsSerial.print("sat "); 
  gpsSerial.println(msg_GPS.numSat);
#endif
}

void updateLatitude(char orientation) {
  //Convert string to float

  char mlat[11] = { 
    '0','0','0','0','0','0','0','0','0','0','0'  };
  for (int t = 0;t < 11;t++) {
    if (((msg_GPS.latitude[t]- '0') >= 0  && (msg_GPS.latitude[t]- '0') <= 9) ||  msg_GPS.latitude[t] == '.') {
      if (msg_GPS.latitude[t] == '.') 
        mlat[t] = '.';
      else 
        mlat[t] = msg_GPS.latitude[t];
    } 
    else break;
  }


  double lat = atof(mlat);
  int head = (int)(lat/100);
  lat =  (lat - (head*100))/60 + head;
  //Convert float to string
  if (orientation == 'N') dtostrf(lat,2,9,msg_GPS.latitude);
  else dtostrf(-lat,2,9,msg_GPS.latitude);
}

void updateLongitude(char orientation) {
  char mlong[11] = { 
    '0','0','0','0','0','0','0','0','0','0','0'  };
  for (int t = 0;t < 11;t++) {
    if (((msg_GPS.longitude[t]- '0') >= 0  && (msg_GPS.longitude[t]- '0') <= 9) ||  msg_GPS.longitude[t] == '.') {
      if (msg_GPS.longitude[t] == '.') 
        mlong[t] = '.';
      else 
        mlong[t] = msg_GPS.longitude[t];
    } 
    else break;
  }
  double  lng = atof(mlong);  
  int head = (int)(lng/1000);
  lng =  (lng - (head*1000))/60.0 + head;

  //Convert float to string
  if (orientation == 'E') dtostrf(lng,2,9,msg_GPS.longitude);
  else  dtostrf(-lng,2,9,msg_GPS.longitude);

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





