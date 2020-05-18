#define reloj 1
/**
    LIBRERIAS NECESARIAS PARA EL FUNCIONAMIENTO DEL CODIGO
*/
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>

#if reloj
  #include <RTClib.h>
#endif
/**
    OBJETOS DE LAS LIBRERIAS
*/
LiquidCrystal_I2C lcd(0x23, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#if reloj
  RTC_DS3231 rtc;
  DateTime now;
#endif
/**
    MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
*/
#define COUNT(x) sizeof(x)/sizeof(*x)                   // Macro para contar el numero de elementos de un array
#define  pENCO_SW    A1                              // Pin encoder SW
#define  pENCO_DT    A3                              // Pin encoder DT
#define  pENCO_CLK   A2                              // Pin encoder CLK
#define  rowsLCD     4                              // Filas del LCD
#define  columnsLCD  20                             // Columnas del LCD
const byte  iARROW = 0;                              // ID icono flecha

const byte bARROW[]   = {                               // Bits icono flecha
  B00000, 
  B00100, 
  B00110, 
  B11111,
  B00110, 
  B00100, 
  B00000, 
  B00000
};

enum Button { Unknown, Ok, Left, Right } btnPressed;    // Enumerador con los diferentes botones disponibles
enum Screen { Menu1, Menu2, Flag, Number };             // Enumerador con los distintos tipos de submenus disponibles


const char *txMENU[] = {                                // Los textos del menu principal, la longitud maxima = columnsLCD-1, rellenar caracteres sobrantes con espacios.
  "1. Atras <<         ",
  "2. INICIAR Muestreo ",
  "3. No. Muestras     ",
  "4. Espaciado cm     ",
  "5. Guardar          "
};

const byte iMENU = COUNT(txMENU);                       // Numero de items/opciones del menu principal


/* ESTRUCTURAS CONFIGURACION */
struct MYDATA {         // Estructura STRUCT con las variables que almacenaran los datos que se guardaran en la memoria EEPROM
  //int sizeCuadre;
  byte initialized;
  byte numeroDeMuestras;
  byte modoAutomatico;
  byte distSensor;
};
union MEMORY {     // Estructura UNION para facilitar la lectura y escritura en la EEPROM de la estructura STRUCT
  MYDATA d;
  byte b[sizeof(MYDATA)];
}
memory;



//****************************************
//definiciones de Funcionamiento
//****************************************
#define DEBUG 1
#define numSensors 5
#define ledOn      8
#define chipSelect 2

//----------------------------------------

//****************************************
//Variables de Funcionamiento
//****************************************
const byte sensor[]={3,4,5,6,7};

volatile bool flag1, flag2, flag3, flag4, flag5;
volatile unsigned int tiemposParciales[numSensors];
volatile int espacioSensores;

byte startSamples=false;

//-----------cronometro------------------//
//unsigned long tActual, tAnterior, tTranscurrido;
//----------------------------------------


void setup() {
  //este delay lop tomamos para evitar saltos o bugs mientra se estabiliza la corriente
  delay(3000);
  Serial.begin(115200);
    
  pinMode(pENCO_SW,  INPUT_PULLUP);
  pinMode(pENCO_DT,  INPUT_PULLUP);
  pinMode(pENCO_CLK, INPUT_PULLUP);
  pinMode(ledOn, OUTPUT);

  //definiendoPines
  /*
  for(byte i = 0; i < numSensors; i++){
    pinMode(sensor[i], INPUT);
  }
  */
  pinMode(3, INPUT); pinMode(4, INPUT); pinMode(5, INPUT); pinMode(6, INPUT); pinMode(7, INPUT);


  //---------------------------------------------
  // Carga la configuracion de la EEPROM, y la configura la primera vez:
  readConfiguration();
  
  // Inicia el LCD:
  lcd.begin(columnsLCD, rowsLCD);
  lcd.createChar(iARROW, bARROW);


  //Iniciar SD
  Serial.print(F("Iniciando SD..."));
  if (!SD.begin(chipSelect)) {
    lcd.setCursor(0, 0);
    lcd.print("Tarjeta Falla, o no esta presente");
    lcd.setCursor(0,3);
    lcd.print("omitir <enter>");
    // don't do anything more:
    while (digitalRead(pENCO_SW));
  }
  Serial.println("card initialized.");

  #if reloj
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  #endif
  
  #if !DEBUG
  // Imprime la informacion del proyecto:
    lcd.setCursor(0, 0); lcd.print("  Sensor  Velocidad ");
    lcd.setCursor(0, 1); lcd.print("     por barrera    ");
    delay (2000);

    lcd.setCursor(0, 2); lcd.print("    Configurando    ");
    lcd.setCursor(0, 3);
    for ( int i = 0 ; i < columnsLCD ; i++ )
    {
      lcd.print(".");
      delay(100);
    }
  #endif


  //Enviar el sistema de calibracion de inmediato
  calibracion();
  delay(1000);
}
byte memoriaTramos = 0;

void loop() {
  // put your main code here, to run repeatedly:
   mostrarPantalla();

   if(startSamples){
     //PONER AQUI Lo del sensor principal
     lcd.print("     Muestra 1      ");
     boolean banderaEscape = 1;
     byte contador = 1;
     
     do{
      if(!digitalRead(pENCO_SW)){
        delay(500);
        lcd.clear();
        //Imprimir en la interfaz
        lcd.print("     Muestra "); lcd.print(contador);

        ++contador;

        //resetear todas las flags para nueva muestra
        flag1=0; flag2=0; flag3=0; flag4=0; flag5=0;


        //Obtener Tiempos
        getTiemposParciales(memoriaTramos);

        
//        Serial.println("obteniendo parciales");
//        Serial.println(contador);
//        delay(1000);
        //Almacenar en memoria SD
        delay(1000);
        Guardar(memoriaTramos);
        delay(1000);


        //Para salir del bucle
        
        if(contador > memory.d.numeroDeMuestras){
          banderaEscape = 0;
        }
        
        //Resetear la memoria de tramos para tomar le indice 0 nuevamente
        memoriaTramos=0;
      }
      //Lectursa general de sensores
      sensores();
     }while(banderaEscape);

     delay(800);
     
     startSamples = 0;
   }
 
  
  
 
}


void mostrarPantalla(){
  static unsigned long tNow      = 0;
  static unsigned long tPrevious = 0;

  tNow = millis();
  btnPressed = readButtons();
  

  //si presiona el boton despliega el menu
  if ( btnPressed == Button::Ok )
    openMenu();

  /*para borrar  
   * 
   */
  else if ( btnPressed == Button::Left )
    calibracion();

  
  // Pinta la pantalla principal cada 1 segundo:
  
  if ( tNow - tPrevious >= 1000 )  {
    tPrevious = tNow;

    
      lcd.clear();
      #if reloj
      now=rtc.now();
      #endif
      lcd.setCursor(0,0);
      lcd.print("  SENSOR VELOCIDAD  ");

      #if reloj
      lcd.setCursor(0,2);
      lcd.print("Fecha: ");
      lcd.print(now.day()); lcd.print("/");
      lcd.print(now.month()); lcd.print("/");
      lcd.print(now.year()); 
      lcd.setCursor(0,3);

      lcd.print("Hora:  ");
      lcd.print(now.hour()); lcd.print(":");
      lcd.print(now.minute()); lcd.print(":");
      lcd.print(now.second());
    
      #endif
  }

}

/**
    MUESTRA EL MENU PRINCIPAL EN EL LCD.
*/
void openMenu() {
  byte idxMenu       = 0;
  boolean exitMenu   = false;
  boolean forcePrint = true;

  lcd.clear();

  while ( !exitMenu )  {
    btnPressed = readButtons();

    if ( btnPressed == Button::Left && idxMenu - 1 >= 0 )    {
      idxMenu--;
    }
    else if ( btnPressed == Button::Right && idxMenu + 1 < iMENU )    {
      idxMenu++;
    }
    else if ( btnPressed == Button::Ok )    {
      switch ( idxMenu )
      {
        //openSubMenu( byte menuID, Screen screen, int *value, int minValue, int maxValue )
          case 0: readConfiguration();  exitMenu = true; break; //Salir y cancelar cambios
          case 1: openSubMenu( idxMenu, Screen::Flag, &startSamples, 0, 1); exitMenu = true; break;
          case 2: openSubMenu( idxMenu, Screen::Number, &memory.d.numeroDeMuestras, 2,50); exitMenu = true; break;
          case 3: openSubMenu( idxMenu, Screen::Number, &memory.d.distSensor, 5, 20); break;
          case 4: writeConfiguration(); exitMenu = true; break; //Salir y guardar
//        case 3: openSubMenu( idxMenu, Screen::Number, &goToAlimentar,    0, 4);exitMenu = true; break; 
//        case 4: openSubMenu( idxMenu, Screen::Number, &memory.d.tamanioCuadro, 80, 200); break;
//        case 5: openSubMenu( idxMenu, Screen::Number, &memory.d.NroCuadros, 0, 20); break;
//        case 6: openSubMenu( idxMenu, Screen::Number, &memory.d.vecesDelGripper, 0, 4); break;
//        case 7: openSubMenu( idxMenu, Screen::Menu1,  &memory.d.time_unit, 0, COUNT(txSMENU1) - 1 ); break;
//        case 8: openSubMenu( idxMenu, Screen::Number, &memory.d.velocidad, 70, 150); break;
//        case 9: openSubMenu( idxMenu, Screen::Number, &memory.d.lineasCargadas, 0, 3); break;
//        case 10: openSubMenu( idxMenu, Screen::Number, &memory.d.numeroDeProduccion, 0, 100 ); break;
//        case 11: openSubMenu( idxMenu, Screen::Number, &memory.d.tiempoGoteo, 0, 1000); break;
//        case 12: openSubMenu( idxMenu, Screen::Number, &memory.d.tiempoHumectacion, 0, 1000); break;
//        
//        case 13: goteoManual(); break;
//        case 14: writeConfiguration(); exitMenu = true; break; //Salir y guardar
//        case 15: lcd.clear();lcd.print("Reset Fabrica?");esperar(3000); 
//                 openSubMenu( idxMenu, Screen::Flag, &restaurarFabricaVar, 0, 1);
//                 restaurarFabrica(restaurarFabricaVar); restaurarFabricaVar=false; 
//                 //realiza  el cambio nuevamente
//                 restaurarFabrica(restaurarFabricaVar); 
//                 exitMenu = true; 
//                 break;
//        case 16: openSubMenu( idxMenu, Screen::Menu2,  &memory.d.modoAutomatico, 0, COUNT(txSMENU2)-1 ); Serial.println(memory.d.modoAutomatico); break;        
          default: break;
      }
      forcePrint = true;
    }


    if ( !exitMenu && (forcePrint || btnPressed != Button::Unknown) )
    {
      forcePrint = false;

      static const byte endFor1 = (iMENU + rowsLCD - 1) / rowsLCD;
      int graphMenu     = 0;

      for ( int i = 1 ; i <= endFor1 ; i++ )
      {
        if ( idxMenu < i * rowsLCD )
        {
          graphMenu = (i - 1) * rowsLCD;
          break;
        }
      }

      byte endFor2 = graphMenu + rowsLCD;

      for ( int i = graphMenu, j = 0; i < endFor2 ; i++, j++ )
      {
        lcd.setCursor(1, j);
        lcd.print( (i < iMENU) ? txMENU[i] : "                    " );
      }

      for ( int i = 0 ; i < rowsLCD ; i++ )
      {
        lcd.setCursor(0, i);
        lcd.print(" ");
      }
      lcd.setCursor(0, idxMenu % rowsLCD );
      lcd.write(iARROW);
    }
  }

  lcd.clear();
}


/**
   MUESTRA EL SUBMENU EN EL LCD.

   @param menuID    ID del menu principal para usarlo como titulo del submenu
   @param screen    Segun el tipo, se representara el submenu de una forma u otra.
   @param value     Puntero a la variable que almacena el dato, y que se modificara.
   @param minValue  Valor minimo que puede tener la variable.
   @param maxValue  Valor maximo que puede tener la variable.
*/
void openSubMenu( byte menuID, Screen screen, byte *value, byte minValue, byte maxValue ) {
  boolean exitSubMenu = false;
  boolean forcePrint  = true;

  lcd.clear();

  while ( !exitSubMenu )
  {
    btnPressed = readButtons();

    if ( btnPressed == Button::Ok )
    {
      exitSubMenu = true;
    }
    else if ( btnPressed == Button::Left && (*value) - 1 >= minValue )
    {
      (*value)--;
    }
    else if ( btnPressed == Button::Right && (*value) + 1 <= maxValue )
    {
      (*value)++;
    }


    if ( !exitSubMenu && (forcePrint || btnPressed != Button::Unknown) )
    {
      forcePrint = false;

      lcd.setCursor(0, 0);
      lcd.print(txMENU[menuID]);

      lcd.setCursor(0, 1);
      lcd.print("<");
      lcd.setCursor(columnsLCD - 1, 1);
      lcd.print(">");

      if ( screen == Screen::Menu1 )
      {
        lcd.setCursor(1, 1);
        //lcd.print(txSMENU1[*value]);
      }
      else if ( screen == Screen::Menu2 )
      {
        lcd.setCursor(1, 1);
        //lcd.print(txSMENU2[*value]);
      }
      else if ( screen == Screen::Flag )
      {
        lcd.setCursor(columnsLCD / 2 - 1, 1);
        lcd.print(*value == 1 ? "SI" : "NO");
      }
      else if ( screen == Screen::Number )
      {
        lcd.setCursor(columnsLCD / 2 - 1, 1);
        lcd.print(*value);
        lcd.print(" ");
      }
    }

  }

  lcd.clear();
}


/**
    LEE (Y CONFIGURA LA PRIMERA VEZ) LA MEMORIA EEPROM CON LA CONFIGURACION DE USUARIO
*/
void readConfiguration()
{
  for ( int i = 0 ; i < sizeof(memory.d) ; i++  )
    memory.b[i] = EEPROM.read(i);

  if ( memory.d.initialized != 'Y' )  {
    Serial.println("entro en initialized");
    memory.d.initialized = 'Y';
    memory.d.numeroDeMuestras = 5;
    memory.d.modoAutomatico=false;
    memory.d.distSensor = 20;
    writeConfiguration();
  }
}


/**
    ESCRIBE LA MEMORIA EEPROM CON AL CONFIGURACION DE USUARIO
*/
void writeConfiguration()
{
  for ( int i = 0 ; i < sizeof(memory.d) ; i++  )
    EEPROM.write( i, memory.b[i] );
}


/**
    LEE LOS DISTINTOS BOTONES DISPONIBLES Y DEVUELVE EL QUE HAYA SIDO PULSADO
        Este bloque de codigo varia dependiendo del tipo de teclado conectado, en mi blog estan
        disponibles los ejemplos para teclados digitales, analogicos, y este para encoder rotatorio.
        Al cambiar de tipo de teclado hay que adaptar tambien el resto de codigo para que haga uso de cada boton segun queramos.
*/
Button readButtons() {
  
  static boolean oldA = HIGH;
  static boolean newA = LOW;
  static boolean newB = LOW;

  btnPressed = Button::Unknown;
  newA = digitalRead(pENCO_DT);
  newB = digitalRead(pENCO_CLK);

  if ( !oldA && newA )
  {
    btnPressed = !newB ? Button::Left : Button::Right;
    delay(50);
  }
  else if ( !digitalRead(pENCO_SW) )
  {
    while (!digitalRead(pENCO_SW));
    btnPressed = Button::Ok;
    delay(50);
  }

  oldA = newA;
  return btnPressed;
}
