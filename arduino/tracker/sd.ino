
#ifdef SDWRITE

void createNewFile() {
    File fileNumber = SD.open("last.txt",  FILE_READ);
    String number = "";
    if (fileNumber) {
        while (fileNumber.available()) {
            number.concat((char)fileNumber.read());
        }
        fileNumber.close();
        int num = number.toInt()+1;
        writeLastNumber(num);
        #ifdef DEBUG
        Serial.print("Numberation file increased: ");
        #endif
        FILENAME = "data";
        FILENAME.concat(num);
        FILENAME.concat(".txt");
        #ifdef DEBUG
        Serial.println(FILENAME);
        #endif
    }  else {
      #ifdef DEBUG
      Serial.println("No numberation file");
      #endif
      FILENAME = "data1.txt";
      writeLastNumber(1);
    }
}

void writeLastNumber(int number) { //O_WRITE | O_CREAT | O_TRUNC
    File fileNumber = SD.open("last.txt", O_WRITE | O_CREAT | O_TRUNC); //Trunc will set the length to 0, equal to erase the file
    if (fileNumber) {
       #ifdef DEBUG
       Serial.println("writing number");
       #endif
       fileNumber.print(number);
    } 
    #ifdef DEBUG
    else Serial.println("fail to write number");
    #endif
    fileNumber.close();
}

#endif
