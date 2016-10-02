
#ifdef SDWRITE

void createNewFile() {
  File fileNumber = SD.open("last.txt",  FILE_READ);
  String number = "";
  if (fileNumber) {
    //Check if the last.txt got the last number and save in the variable
    while (fileNumber.available()) {
      number.concat((char)fileNumber.read());
    }
    fileNumber.close();
    int num = number.toInt();

    //Fix last data record
    FILENAME = "data";
    FILENAME.concat(num);
    FILENAME.concat(".txt");
    writeJSONTag(0);

    //Increase the number
    num += 1;

    //Write the new number into last.txt
    writeLastNumber(num);
#ifdef DEBUG
    gpsSerial.print("Numberation file increased: ");
#endif
    //Define the new filename
    FILENAME = "data";
    FILENAME.concat(num);
    FILENAME.concat(".txt");
    writeJSONTag(1);
#ifdef DEBUG
    gpsSerial.println(FILENAME);
#endif
  }  
  else {
#ifdef DEBUG
    gpsSerial.println("No numberation file");
#endif
    FILENAME = "data1.txt";
    writeJSONTag(1);
    writeLastNumber(1);
  }
  isFirstWrite = true;
}


void writeJSONTag(int position) {
  char tmpFile[FILENAME.length()+1];
  FILENAME.toCharArray(tmpFile, sizeof(tmpFile));
  File dataFile = SD.open(tmpFile,  O_CREAT | O_APPEND | O_WRITE); //Append the file

  if (dataFile) {
    if (position == 1) //Write beginning of the tag
      dataFile.println("[");
    else //Write end of the tag
    dataFile.println("]");
  }  
  dataFile.close();
}




void writeLastNumber(int number) { //O_WRITE | O_CREAT | O_TRUNC
  File fileNumber = SD.open("last.txt", O_WRITE | O_CREAT | O_TRUNC); //Trunc will set the length to 0, equal to erase the file
  if (fileNumber) {
#ifdef DEBUG
    gpsSerial.println("writing number");
#endif
    fileNumber.print(number);
  } 
#ifdef DEBUG
  else gpsSerial.println("fail to write number");
#endif
  fileNumber.close();
}

void initSD() {
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(chipSelect, OUTPUT);  
  if (!SD.begin(chipSelect)) {
    ledColor(25,0,0); //Solid led means something wrong
#ifdef DEBUG
    gpsSerial.println("Card failed, or not present");
#endif
    // don't do anything more:
    while(true);

  }
#ifdef DEBUG
  gpsSerial.println("card initialized.");
#endif
}

#endif


void sendToGPS() {
  #ifdef SDWRITE
  File myFile = SD.open("gps.ubx");
  if (myFile) {
      byte b = 0;
      while (myFile.available()) {
         b = myFile.read();
         //if (b == 0xB5) 
         //  Serial.println();
    	 //Serial.print(b,HEX);
         Serial.write(b);
      }
     
  } 
  myFile.close();
  #endif

}



