#include <LiquidCrystal.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
RTC_DS1307 RTC;
const int DHTPin = 5;     // what digital pin we're connected to
 
DHT dht(DHTPin, DHTTYPE);

 
File logFile;
 
void setup()
{
  lcd.begin(20, 4);
  lcd.print("www.ajpdsoft.com");
  dht.begin();
  Wire.begin(); // Inicia el puerto I2C
  RTC.begin(); // Inicia la comunicación con el RTC 
  Serial.begin(9600);
  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(9))
  {
    Serial.println(F("Error al iniciar"));
    return;
  }
  Serial.println(F("Iniciado correctamente"));
}
 
 
 
void loop()
{
   float h = dht.readHumidity();
   float t = dht.readTemperature();
    DateTime now = RTC.now();
  // Abrir archivo y escribir valor
  logFile = SD.open("datalog.txt", FILE_WRITE);
  
  if (logFile) { 
        logFile.print(String("Fecha: ") + now.day() + String("/") + now.month() + String("/") + now.year());
        logFile.print(String("   Hora: ") + now.hour() + String(":") + now.minute() + String(":") + now.second());
        logFile.println(String("   Temperatura: ") + float(t) + String("  Humedad: ") + float(h));
        logFile.close();
  
  } 
  else {
    Serial.println("Error al abrir el archivo");
  }
  delay(1000);
}
