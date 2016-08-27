void initBT() {
  BT_ENABLE = true;
}

void readCommand() {
  String command = "";
  if (gpsSerial.available()) {
    while(gpsSerial.available() > 0) {
      command += (char)gpsSerial.read();
      delay(10);
    }
  }
  if (command != "") {        
    if (command.indexOf("send") >= 0) {       
      sendFile();        
    } 
    else if  (command.indexOf("stop") >= 0) {
      stopBT();
      gpsSerial.println("bye!");
    } 
    else if  (command.indexOf("new") >= 0) {
#ifdef SDWRITE
      createNewFile();
      gpsSerial.print("New file name: ");
      gpsSerial.println(FILENAME);
#endif
    }
  }
}

void stopBT() {
  BT_ENABLE = false;
}

void sendFile() {
#ifdef SDWRITE
  char tmpFile[FILENAME.length()+1];
  FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
  File fileNumber = SD.open(tmpFile,  FILE_READ);
  if (fileNumber) {
    gpsSerial.println("start print");
    while (fileNumber.available()) {
      gpsSerial.print((char)fileNumber.read());
    }
    //Send the } because it will not write it when button is on only at the new creation of the file
    gpsSerial.println("]");
    gpsSerial.println("end");
  } 
  else 
    gpsSerial.println("error");
  fileNumber.close();
#endif
}

