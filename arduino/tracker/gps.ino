
void readGPS()
{
 
 while (gpsSerial.available() > 0) {
   //Serial.print((char)gpsSerial.read());
    if (gps.encode(gpsSerial.read()))
        gps_ready = true;
    else 
        gps_ready = false;
 }
      
}


