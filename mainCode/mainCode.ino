#include "DHT.h"
#define DHTPIN 17
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
float h, t;

#include <SD.h>
#include <Wire.h>
File logFile;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x23, 20, 4);

#include <RTClib.h>
RTC_DS3231 rtc;
DateTime now;

// Cronómetro
bool Flag1, Flag2, Flag3, Flag4, Flag5, Flag6;
String TP1,TP2,TP3,TP4,TP5,TP6;
unsigned long inicio, finalizado, Ttranscurrido;

void setup() {

  Serial.begin(9600);
  Serial.println("Inicializar...");
  dht.begin();
  Wire.begin(); // Inicia el puerto I2C

  lcd.init();
  lcd.backlight();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.print(F("Iniciando SD ..."));
  if (!SD.begin(53))  {
    Serial.println(F("Error al iniciar"));
    return;
  }
  Serial.println(F("Iniciado correctamente"));


  pinMode(A0, INPUT_PULLUP); // Boton de inicio
  pinMode(A1, INPUT_PULLUP); // TP1
  pinMode(A2, INPUT_PULLUP); // TP2
  pinMode(A3, INPUT_PULLUP); // TP3
  pinMode(A4, INPUT_PULLUP); // TP4
  pinMode(A5, INPUT_PULLUP); // TP5
  pinMode(A6, INPUT_PULLUP); // TP6/RESET

}



void loop() {

  Sensores();
  now = rtc.now();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  h = dht.readHumidity();
  t = dht.readTemperature();


}

void Sensores() {
  if ((analogRead(A0) <= 500 ) and (Flag1 == 0) and (Flag2 == 0) and (Flag3 == 0) and (Flag4 == 0) and (Flag5 == 0) and (Flag6 == 0)) {
    inicio = millis();
    delay(300); //  por rebote
    Serial.println("Inicializar...");
    Flag1 = 1;
    Serial.print(String("Fecha: ") + now.day() + String("/") + now.month() + String("/") + now.year());
    Serial.print(String("   Hora: ") + now.hour() + String(":") + now.minute() + String(":") + now.second());
    Serial.println(String("   Temperatura: ") + float(t) + String("  Humedad: ") + float(h));
  }
  if ((analogRead(A1) <= 500) and (Flag1 == 1) )  {
    finalizado = millis();
    delay(300); // por rebote
    MostrarResultado();
    Flag1 = 0; Flag2 = 1;
    Serial.println("Flag1...");
  }
  if ((analogRead(A2) <= 500) and (Flag2 == 1) )  {
    finalizado = millis();
    delay(300); // por rebote
    MostrarResultado();
    Flag2 = 0; Flag3 = 1;
    Serial.println("Flag2...");
  }
  if ((analogRead(A3) <= 500) and (Flag3 == 1) )  {
    finalizado = millis();
    delay(300); // por rebote
    MostrarResultado();
    Flag3 = 0; Flag4 = 1;
    Serial.println("Flag3...");
  }
  if ((analogRead(A4) <= 500) and (Flag4 == 1) )  {
    finalizado = millis();
    delay(300); // por rebote
    MostrarResultado();
    Flag4 = 0; Flag5 = 1;
    Serial.println("Flag4...");
  }
  if ((analogRead(A5) <= 500) and (Flag5 == 1) )  {
    finalizado = millis();
    delay(300); // por rebote
    MostrarResultado();
    Flag5 = 0; Flag6 = 1;
    Serial.println("Flag5...");
  }

  if ((analogRead(A6) <= 500) and (Flag1 == 0) and (Flag2 == 0) and (Flag3 == 0) and (Flag4 == 0) and (Flag5 == 0) and (Flag6 == 1)) {
    finalizado = millis();
    delay(300); // por rebote
    MostrarResultado();
    Flag6 = 0;
    inicio = millis();
    Serial.println("Flag6... y reset");
    Grabar();
  }
}

void MostrarResultado() {
  float h, m, s, ms;
  unsigned long terminado;
  Ttranscurrido = finalizado - inicio;
  h = int(Ttranscurrido / 3600000);
  terminado = Ttranscurrido % 3600000;
  m = int(terminado / 60000);
  terminado = terminado % 60000;
  s = int(terminado / 1000);
  ms = terminado % 1000;
  Serial.print("Tiempo transcurrido : ");
  Serial.println(Ttranscurrido);
  Serial.print("Tiempo transcurrido detallado: ");
  Serial.print(m, 0);  Serial.print("m ");
  Serial.print(s, 0);  Serial.print("s ");
  Serial.print(ms, 0); Serial.println("ms");
  Serial.println();
}

void Grabar() {

  logFile = SD.open("datalog.txt", FILE_WRITE);

  if (logFile) {
    logFile.print(String("Fecha: ") + now.day() + String("/") + now.month() + String("/") + now.year());
    logFile.print(String("   Hora: ") + now.hour() + String(":") + now.minute() + String(":") + now.second());
    logFile.print(String("   Temperatura: ") + float(t) + String("  Humedad: ") + float(h));
    
    logFile.close();
    Serial.println("Guardado el archivo");

  }
  else {
    Serial.println("Error al abrir el archivo");
  }
  delay(1000);
}
