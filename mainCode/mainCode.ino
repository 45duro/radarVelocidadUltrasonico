#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <RTClib.h>

#define SONAR_NUM     2 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 50 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).
#define ITERATIONS     5 // Number of iterations.
#define numSensors 4


//unidades poara facilitar medida
unsigned long mitiempo = 0;
unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.
int posAnterior[SONAR_NUM], posActual[SONAR_NUM];



int medidaPosicion[2];
float medidaVelocidad[2];

boolean flags[numSensors], memorias[numSensors];

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(7, 6, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(8, 9, MAX_DISTANCE),
  
};
LiquidCrystal_I2C lcd(0x23,20,4);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;

  //CONVERTIR SENSORES EN ENTRADAS
  for(byte i = 0; i <= numSensors;i++){
    pinMode(i+14, INPUT);
  }

}

void loop() {

  lectura_Sensor();
  for(byte i = 0; i < numSensors; i++ ){
    if(flags[i])
      memorias[i]=1;
  }

  
    
 
  
  
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  int derivada = SONAR_NUM*PING_INTERVAL;
  float flag =0;
  //Asignacion de tiempos
  for(byte i = 0;  i < SONAR_NUM; i++){
    posActual[i] = cm[i];
    flag = float(posActual[i]-posAnterior[i])/derivada;

    medidaPosicion[i] = posActual[i];
    medidaVelocidad[i]= (abs(flag),3);
    Serial.print("\tpos"); Serial.print(i); Serial.print(": "); Serial.print(posActual[i]);
    Serial.print("\tVel"); Serial.print(i); Serial.print(": "); Serial.print(abs(flag),3);
    
  }

  Serial.println();
  
  
}


void medirConRadar(){
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
    posAnterior[i] = posActual[i];
  }
}

void lectura_Sensor(){
 
  for(byte i = 0; i < numSensors; i++){
    //Estas lineas son para borrrar solamente es la prueba
    if(i == 0 || i == 3)
      digitalRead(i+14)? flags[i]=0: flags[i]=1 ;
    else
      flags[i]=digitalRead(i+14); 
  }

  for(byte i = 0; i < numSensors; i++){
    lcd.setCursor(0,i);
    lcd.print(flags[i]); 
  }
 
}
