void initBT() {
  Serial2.begin(9600);
  BT_ENABLE = true;
}

void readCommand() {
  String command = "";
  if (Serial2.available()) {
      while(Serial2.available() > 0) {
            command += (char)Serial2.read();
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
  Serial2.end();
  BT_ENABLE = false;
}

void sendFile() {
  #ifdef SDWRITE
  char tmpFile[FILENAME.length()+1];
  FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
  File fileNumber = SD.open(tmpFile,  FILE_READ);
  if (fileNumber) {
     Serial2.println("start print");
      while (fileNumber.available()) {
          Serial2.print((char)fileNumber.read());
      }
      Serial2.println("end");
  } else 
     Serial2.println("error");
  fileNumber.close();
  #endif
}
