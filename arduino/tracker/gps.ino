
void readGPS()
{
 
 while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read()))
        gps_ready = true;
    else 
        gps_ready = false;
 }
      
}


