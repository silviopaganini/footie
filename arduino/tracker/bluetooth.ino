void initBT() {
  BT_ENABLE = true;
}

void readCommand() {
  String command = "";
  if (Serial.available()) {
      while(Serial.available() > 0) {
            command += (char)Serial.read();
            delay(10);
      }
  }
  if (command != "") {        
    if (command.indexOf("send") >= 0) {       
        sendFile();        
    } else if  (command.indexOf("stop") >= 0) {
        stopBT();
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
     Serial.println("start print");
      while (fileNumber.available()) {
          Serial.print((char)fileNumber.read());
      }
      Serial.println("end");
  } else 
     Serial.println("error");
  fileNumber.close();
  #endif
}
