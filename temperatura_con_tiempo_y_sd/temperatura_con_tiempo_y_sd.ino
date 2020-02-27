#include "DHT.h"
#include <SPI.h>
#include <SD.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;
File myFile;


void setup() {
    dht.begin();
  Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicación con el RTC
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(9)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
         DateTime now = RTC.now(); // Obtiene la fecha y hora del RTC
   float h = dht.readHumidity();
  float t = dht.readTemperature();
  float F = dht.readTemperature(true);

    if (isnan(h) or isnan(t) or isnan(F)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("------------------------------------------------------------------");
    delay(1000);
    // close the file:
    myFile.close();
    Serial.println("doneeeeeeee.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }



}

void loop() {


}
