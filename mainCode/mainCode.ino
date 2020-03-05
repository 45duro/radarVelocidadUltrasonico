#include <HCSR04.h>
HCSR04 hc(A9, new int[2] {A0, A1}, 2); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)

#include "DHT.h"
#define DHTPIN 17
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


float h, t;

#include <SD.h>
#include <Wire.h>
File logFile; byte contadorParciales = 0; 



#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x23, 20, 4);

#include <RTClib.h>
RTC_DS3231 rtc;
DateTime now;

// Cronómetro
bool Flag1, Flag2, Flag3, Flag4, Flag5, Flag6;
String TP1,TP2,TP3,TP4,TP5,TP6;
unsigned long tParciales[6];// 6 porque son 6 banderas y seis veces almacenara el resultado
unsigned long inicio, finalizado, Ttranscurrido;
unsigned long TiempoAhora = 0;

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


  //pinMode(A0, INPUT_PULLUP); // Boton de inicio
  //pinMode(A1, INPUT_PULLUP); // TP1
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
  if ((Flag1 == 0) and (Flag2 == 0) and (Flag3 == 0) and (Flag4 == 0) and (Flag5 == 0) and (Flag6 == 0) and (radarSensorBool(1, 20.0))) {
    inicio = millis();
    //esperar(1); //  por rebote
    Serial.println("Inicializar...");
    Flag1 = 1;
    Serial.print(String("Fecha: ") + now.day() + String("/") + now.month() + String("/") + now.year());
    Serial.print(String("   Hora: ") + now.hour() + String(":") + now.minute() + String(":") + now.second());
    Serial.println(String("   Temperatura: ") + float(t) + String("  Humedad: ") + float(h));
  }
  else if ((Flag1 == 1) and (radarSensorBool(0, 20.0)))  {
    finalizado = millis();
    //esperar(0); // por rebote
    MostrarResultado();
    Flag1 = 0; Flag2 = 1;
    Serial.println("Flag1...");
  }
  else if ((analogRead(A2) <= 500) and (Flag2 == 1) )  {
    finalizado = millis();
    esperar(300); // por rebote
    MostrarResultado();
    Flag2 = 0; Flag3 = 1;
    Serial.println("Flag2...");
  }
  else if ((analogRead(A3) <= 500) and (Flag3 == 1) )  {
    finalizado = millis();
    esperar(300); // por rebote
    MostrarResultado();
    Flag3 = 0; Flag4 = 1;
    Serial.println("Flag3...");
  }
  else if ((analogRead(A4) <= 500) and (Flag4 == 1) )  {
    finalizado = millis();
    esperar(300); // por rebote
    MostrarResultado();
    Flag4 = 0; Flag5 = 1;
    Serial.println("Flag4...");
  }
  else if ((analogRead(A5) <= 500) and (Flag5 == 1) )  {
    finalizado = millis();
    esperar(300); // por rebote
    MostrarResultado();
    Flag5 = 0; Flag6 = 1;
    Serial.println("Flag5...");
  }

  else if ((analogRead(A6) <= 500) and (Flag1 == 0) and (Flag2 == 0) and (Flag3 == 0) and (Flag4 == 0) and (Flag5 == 0) and (Flag6 == 1)) {
    finalizado = millis();
    esperar(300); // por rebote
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

  //Guardar en la variable para su posterior llamado
  tParciales[contadorParciales] = Ttranscurrido;
  
  Serial.print("Tiempo transcurrido detallado: ");
  Serial.print(m, 0);  Serial.print("m ");
  Serial.print(s, 0);  Serial.print("s ");
  Serial.print(ms, 0); Serial.println("ms");
  Serial.println();
  //Para que se reinicie el contador y no supere los 6 sensores
  contadorParciales >= 6? contadorParciales=0:contadorParciales++;
  
}

void Grabar() {

  logFile = SD.open("datalog.txt", FILE_WRITE);

  if (logFile) {
    logFile.print(String("Fecha:\t") + now.day() + String("/") + now.month() + String("/") + now.year());
    logFile.print(String("\tHora:\t") + now.hour() + String(":") + now.minute() + String(":") + now.second());
    logFile.print(String("\tTemperatura:\t") + float(t) + String("\tHumedad:\t") + float(h));
    for(byte i = 0; i < 6; i++){ 
      Serial.println("impresion de parciales");
      Serial.print(String("p") + int(i) + String(" ") + tParciales[i]);
      logFile.print(String("\tp") + i + String(":\t") + tParciales[i]);
    }
    logFile.println();
    logFile.close();
    Serial.println("Guardado el archivo");

  }
  else {
    Serial.println("Error al abrir el archivo");
  }
  esperar(1000);
}

void esperar(int periodo) {
  TiempoAhora = millis();
  while (millis() < TiempoAhora + periodo) {

  }
}

boolean radarSensorBool(byte numSensor, float comparador) {
  esperar(50);
  float medida = hc.dist(numSensor);
  Serial.println(medida);// Serial.print("\t");
  //medida <= 19.5 ? digitalWrite(40, 1) : digitalWrite(40, 0);
  if (medida <= comparador){
    //Serial.print("acierto");
    return true;
  }
  else{
    //Serial.print("NO acierto");
    return false;
  }
}
