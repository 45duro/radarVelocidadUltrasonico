#include <Wire.h> 
#include <RTClib.h>
#include <QTRSensors.h>
#include <LiquidCrystal_I2C.h>

//Configuracion de Sensores
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

//unidades poara facilitar medida
unsigned long mitiempo = 0, tiempoInicial = 0, currentTime[SensorCount];
boolean memoria[SensorCount];
//objetos
LiquidCrystal_I2C lcd(0x23,20,4);
QTRSensors qtr;


void setup() {

  //Inicia comunicación
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("Calibrando");
  // configure the sensors y para mejorar con array
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3}, SensorCount);

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode

  //METER AQUI UN PULSADOR DE CALIBRACION PTE
  /* analogRead() takes about 0.1 ms on an AVR.
  // 0.1 ms per sensor * 4 samples per sensor read (default) * 6 sensors
  // * 10 reads per calibrate() call = ~24 ms per calibrate() call.
  // Call calibrate() 400 times to make calibration take about 10 seconds.
  */
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration

  /* print the calibration minimum values measured when emitters were on
  */
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.minimum[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(qtr.calibrationOn.maximum[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);

}

void loop() {

 //OBTENER VALORES
 int16_t position = qtr.readLineBlack(sensorValues);
 int16_t *posicion = &position;
 
 //IMPRIMIR VALORES
 for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println(position);

  tiempoInicial = millis();
  
  if(posicion >0 && posicion <= qtr.calibrationOn.maximum[0]){
     currentTime[0]=millis() - tiempoInicial; 
     memoria[0] = 1;  
  }
  else if(posicion >1000 && posicion <= qtr.calibrationOn.maximum[1]+1000){
    currentTime[1]=millis() - currentTime[0]; 
     memoria[1] = 1;  
  }
  else if(posicion >2000 && posicion <= qtr.calibrationOn.maximum[2]+2000){
    currentTime[2]=millis() - currentTime[1]; 
     memoria[2] = 1;  
  }
  else if(posicion >3000 && posicion <= qtr.calibrationOn.maximum[3]+3000){
    currentTime[3]=millis() - currentTime[1]; 
     memoria[3] = 1;  
  }
  
  

}
