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
        Serial.println("bye!");
    } else if  (command.indexOf("new") >= 0) {
        createNewFile();
        Serial.print("New file name: ");
        Serial.println(FILENAME);
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
      //Send the } because it will not write it when button is on only at the new creation of the file
      Serial.println("}");
      Serial.println("end");
  } else 
     Serial.println("error");
  fileNumber.close();
  #endif
}
