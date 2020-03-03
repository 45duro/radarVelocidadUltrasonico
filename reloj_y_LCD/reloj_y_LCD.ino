#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x23,20,4);


String daysOfTheWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
void setup() {

  Serial.begin(57600);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("God is nice");
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }

}

void loop() {
  lcd.clear();

  DateTime now = rtc.now();
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print(" (");
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(") ");
  lcd.setCursor(1,1);
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  lcd.print(now.second());
  delay(3000);
}
