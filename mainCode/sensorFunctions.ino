



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

void esperar(int periodo){
  static unsigned long TiempoAhora = millis();
  while(millis() < TiempoAhora+periodo){
    
  }   
}


void sensores(){
  if(!flag1 and !flag2 and !flag3 and !flag4 and !digitalRead(sensor[0])){
    tActual = millis();
    tiemposParciales[0]=tActual;
    flag1 = 1;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 1 ");
    
  }
  else if(flag1 and !digitalRead(sensor[1])){
    tActual = millis();
    flag1 = 0; flag2 = 1;
    tiemposParciales[1]= tActual;

    memoriaTramos++;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 2 ");
  }
  else if(flag2 and !digitalRead(sensor[2])){
    tActual = millis();
    flag2 = 0; flag3 = 1;
    tiemposParciales[2]= tActual;
    memoriaTramos++;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 3 ");
  }
  
  else if(flag3 and !digitalRead(sensor[3])){
    tActual = millis();
    flag3 = 0; flag4 = 1;
    tiemposParciales[3]= tActual;
    memoriaTramos++;
    lcd.clear();
    lcd.setCursor(0,4);
    lcd.print("  Tramo 4 ");
  }
  else if(flag4 and !digitalRead(sensor[4])){
    tActual = millis();
    //Reiniciar variables
    flag4=0;
    tiemposParciales[4]= tActual;
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
    tiemposParcialesFiltrados[i]=0;
  }
  
  for(byte i = 0; i < valorTramo; i++){
      tiemposParcialesFiltrados[i]=tiemposParciales[i+1]-tiemposParciales[i];
      Serial.print(tiemposParcialesFiltrados[i]);
      Serial.print("\t");
  }
    Serial.println();
    Serial.println();
}


void funcionPrincipalSensor(char dato){

  if(dato='t'){
    
  }
  
}




void Guardar(){
  
}
