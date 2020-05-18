void calibracion(){
  boolean bandera = 1;
  do{
    //lcd.setCursor(0,0); lcd.print("Verifique que todos ");
    //lcd.setCursor(0,1); lcd.print("los Sensores esten 1");
    lcd.setCursor(0,0);
    lcd.print(" Enter   <Continua> ");
    //prender los infrarojos
    digitalWrite(ledOn, 1);
    //Lectura de lossensores
    lcd.setCursor(0,1);
    lcd.print(" S1  S2  S3  S4  S5");
    lcd.setCursor(0,3);
    lcd.print("|");
    lcd.setCursor(1,3);
    
    for(byte i=0; i < numSensors; i++){
      lcd.print(digitalRead(sensor[i]));
      lcd.print(" | ");
    }
    bandera = digitalRead(pENCO_SW);
  }while(bandera);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Configuracion    ");
  lcd.print("      Aceptada      ");
}

/*
void esperar(int periodo){
  volatile unsigned long TiempoAhora = millis();
  while(millis() < TiempoAhora+periodo){
    
  }   
}
*/

void sensores(){
  if(!flag1 and !flag2 and !flag3 and !flag4 and !digitalRead(sensor[0])){
    tiemposParciales[0] = millis();
    flag1 = 1;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 1 ");
    
  }
  else if(flag1 and !digitalRead(sensor[1])){
    tiemposParciales[1] = millis();
    flag1 = 0; flag2 = 1;
    memoriaTramos++;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 2 ");
  }
  else if(flag2 and !digitalRead(sensor[2])){
    tiemposParciales[2] = millis();
    flag2 = 0; flag3 = 1;
    memoriaTramos++;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 3 ");
  }
  
  else if(flag3 and !digitalRead(sensor[3])){
    tiemposParciales[3] = millis();
    flag3 = 0; flag4 = 1;
    memoriaTramos++;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 4 ");
  }
  else if(flag4 and !digitalRead(sensor[4])){
    tiemposParciales[4] = millis();
    //Reiniciar variables
    flag4=0;
    memoriaTramos++;
    //Serial.println(tiemposParciales[4]);
    //Serial.println(tActual);
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("Recorrido Completo.");

    
    
  }
  
  
}

void getTiemposParciales(byte valorTramo){
  
  //Limpiar primero el array
  for(byte i = 0; i < numSensors; i++){
    Serial.println(tiemposParciales[i]);
  }
  
  for(byte i = 0; i < valorTramo; i++){
      tiemposParciales[i]=tiemposParciales[i+1]-tiemposParciales[i];
      Serial.print(tiemposParciales[i]);
      Serial.print("\t");
  }
  Serial.println();
  /*
  int minimo=tiemposParciales[0];
  //Definir tramo mas rapido
  for(byte i = 0; i < valorTramo; i++){
    if(tiemposParciales[i]< minimo){
      minimo = tiemposParciales[i];
    }
  }
  float velocidad= (memory.d.distSensor/float(minimo));
  velocidad*=10;
  lcd.setCursor(0,1);
  lcd.print("Vel  max: ");
  lcd.print(velocidad);
  lcd.print(" m/s");
  delay(1000);
  */
}







void Guardar(byte valorTramo){
  
  //char buf4[] = "MM-DD-YYYY\thh:mm:ss";
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
  if(dataFile){

    Serial.println("imprimiendo en SD");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Muestra: ");
    //dataFile.write(now.toString(buf4)); dataFile.print("\t");
 
    
    for(byte i = 0; i < valorTramo; i++){
      float velocidad= (memory.d.distSensor/float(tiemposParciales[i]));
      velocidad*=10;
      dataFile.print(velocidad,6);
      //Velocidad
      dataFile.print("\t");
      
      lcd.setCursor(10,i);
      lcd.print(velocidad);
      lcd.print(" m/s");
    }
    dataFile.println();
    delay(3000);
    dataFile.close();
  }
  else{
    Serial.println("error abriendo datalog.txt");
  }

    
  /*
  int minimo=tiemposParciales[0];
    //Definir tramo mas rapido
  for(byte i = 0; i < valorTramo; i++){
    if(tiemposParciales[i]< minimo){
      minimo = tiemposParciales[i];
    }
  }

  lcd.setCursor(0,1);
  lcd.print("Vel  max: ");
  lcd.print(velocidad);
  lcd.print(" m/s");
  delay(1000);
    
  
  
  
  /*
  Serial.print(now.day() + String("/") + now.month() + String("/") + now.year());
  Serial.print(String("\t") + now.hour() + String(":") + now.minute() + String(":") + now.second());
  Serial.print("\t");
  */
   /*
  
   if(dataFile){
    
    Serial.print(now.day() + String("/") + now.month() + String("/") + now.year());
    Serial.print(String("\t") + now.hour() + String(":") + now.minute() + String(":") + now.second());
    Serial.print("\t");
    
    dataFile.print(now.day() + String("/") + now.month() + String("/") + now.year());
    dataFile.print(String("\t") + now.hour() + String(":") + now.minute() + String(":") + now.second());
    dataFile.print("\t");
    
    for(byte i = 0; i < valorTramo; i++){
      float velocidad= (memory.d.distSensor/float(tiemposParciales[i]));
      velocidad*=10;      
      Serial.print(velocidad,6);
      Serial.print("\t");

      dataFile.print(velocidad,6);
      dataFile.print("\t");
      delay(1);
    }
    dataFile.println();
    Serial.println();
  }

  delay(1);
  dataFile.close();

  */
 
  
  

}
