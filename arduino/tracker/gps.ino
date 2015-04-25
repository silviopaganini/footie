
void readGPS()
{
 
 while (Serial1.available() > 0) {
    if (gps.encode(Serial1.read()))
        gps_ready = true;
    else 
        gps_ready = false;
 }
      
}


