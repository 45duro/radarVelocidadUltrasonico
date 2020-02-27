#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
RTC_DS1307 RTC;
String todo;

File myFile;

void setup() {
  Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicación con el RTC 
  Serial.begin(9600);
  while (!Serial) {
    ;
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(9)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");



}

void loop() {
  DateTime now = RTC.now();
  
  // nothing happens after setup
    myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println(now.second(), DEC);;
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error abriendo el archivo TXT");
  }
}
