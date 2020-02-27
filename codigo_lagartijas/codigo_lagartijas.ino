int sensor1A = 53;
int sensor2A = 51;
int sensor3A = 49;
int sensor4A = 47;
int sensor5A = 45;
int led1 = 38;
int led2 = 40;
int led3 = 42;
int led4 = 44;
int led5 = 46; 
int cron = 0;
int val = 1;
int sensor1;
int sensor2;
int sensor3;
int sensor4;
int sensor5;
void setup() {
  
  Serial.begin(9600);

  sensor1 = digitalRead(53);
  sensor2 = digitalRead(51);
  sensor3 = digitalRead(49);
  sensor4 = digitalRead(47);
  sensor5 = digitalRead(45);
  pinMode(sensor1A, INPUT_PULLUP);
  pinMode(sensor2A, INPUT_PULLUP);
  pinMode(sensor3A, INPUT_PULLUP);
  pinMode(sensor4A, INPUT_PULLUP);
  pinMode(sensor5A, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

}

void loop() {
  if (sensor1 == LOW && val == 1) {
    iniciar();
  } else if (sensor2 == LOW && val == 2) {
    dos();
  } else if (sensor3 == LOW && val == 3) {
    tres();
  } else if (sensor4 == LOW && val == 4) {
    cuatro();
  } else if (sensor5 == LOW && val == 5) {
    cinco();
  }
}

void iniciar () {
  val = 1;
  cron++;
  mostrar();
  delay(1000);

}
void dos () {
  val = 2;
  cron++;
  mostrar();
  delay(1000);
}

void tres () {
  val = 3;
  cron++;
    mostrar();
  delay(1000);
}
void cuatro () {
  val = 4;
  cron++;
    mostrar();
  delay(1000);
}
void cinco () {
  val = 5;
  cron++;
    mostrar();
  delay(1000);
}

void mostrar() {
  Serial.print("El tiempo que se alcanzo en el sensor ");
  Serial.print(val);
Serial.print(" Es de : ");
Serial.println(cron);
}
