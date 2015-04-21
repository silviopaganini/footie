#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <stdio.h>

#include <SPI.h>
#include <SD.h>

// GPS Pins
#define GPS_RX 8
#define GPS_TX 9

// SD Pins
#define SD_MOSI 11
#define SD_MISO 12
#define SD_CLK 5
#define SD_CS 4

// LED
#define LED 13

// BLUETOOTH
#define BT_TX 2
#define BT_RX 3

TinyGPS gps;
SoftwareSerial ss(GPS_RX, GPS_TX);
SoftwareSerial bt(BT_RX, BT_TX);
File gpsData;
bool hardFail = false;

static void smartdelay(unsigned long ms);
static void print_int(unsigned long val, unsigned long invalid, int len);
static char* print_date(TinyGPS &gps);

int state = 0;
int flag = 0;   


void setup()
{
  pinMode(LED, OUTPUT);

  // begin serial for GPS and SD

  Serial.begin(115200);
  ss.begin(9600);

  // Check SD
  if (!SD.begin(SD_CS)) {
    hardFail = true;
    Serial.println("initialization failed!");
    return;
  }
  
}

void loop()
{
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  
  if(bt.available() > 0){
      state = mySerial.read();
      flag=0; //clear the flag so we can print the state
    }
    // if the state is '0' the LED will turn off
    if (state == '0') {
        digitalWrite(LED, LOW);
        if(flag == 0){
          bt.println("LED: off");
          flag = 1;
        }
    }
    // if the state is '1' the led will turn on
    else if (state == '1') {
        digitalWrite(LED, HIGH);
        if(flag == 0){
          bt.println("LED: on");
          flag = 1;
        }
    }
  
//  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
//  print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);

  gps.f_get_position(&flat, &flon, &age);

  // print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  
  // print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
  // print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  // print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);

  // gps.stats(&chars, &sentences, &failed);
  // print_int(chars, 0xFFFFFFFF, 6);
  // print_int(sentences, 0xFFFFFFFF, 10);
  // print_int(failed, 0xFFFFFFFF, 9);

  String jsonString = "{\"lat\":\"";
    jsonString += printFloat(flat, 6);
    jsonString +="\",\"lng\":\"";
    jsonString += printFloat(flon, 6);
    jsonString +="\",\"age\":\"";
    jsonString += printFloat(age, 2);
    jsonString +="\",\"date\":\"";
    jsonString += print_date(gps);
    jsonString +="\"}";

  Serial.println(jsonString);

  gpsData = SD.open("GPS.txt", FILE_WRITE);
  if (gpsData) {
    blinkLed();
    gpsData.println(jsonString);
    gpsData.close();
    hardFail = false;
  } else {
    Serial.println("file not opened");
    hardFail = true;
  }

  smartdelay(1000);
  delay(2000);
}


// HELPERS

void blinkLed()
{
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(500);  
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
//  analogWrite(PIN_R, red);
//  analogWrite(PIN_G, green);
//  analogWrite(PIN_B, blue);  
}

char* printFloat(float num, int len)
{
  char r[32];
  if(num != TinyGPS::GPS_INVALID_F_ANGLE) dtostrf(num, 2, len, r);
  smartdelay(0);
  return r;
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static char* print_date(TinyGPS &gps)
{
  char r[32];
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age != TinyGPS::GPS_INVALID_AGE) {
    sprintf(r, "%02d/%02d/%02d %02d:%02d:%02d ",
            month, day, year, hour, minute, second);
  }

  smartdelay(0);
  return r;
}
