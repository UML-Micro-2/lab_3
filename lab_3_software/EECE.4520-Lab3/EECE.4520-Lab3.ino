#include <LiquidCrystal.h>
#include <Wire.h>
#include "DS1307.h"


#define MOTOR_ENABLE 13
#define MOTOR_CW 11
#define MOTOR_CCW 12
#define MOTOR_OFF 0
#define MOTOR_OFF 0
#define MOTOR_HALF 127
#define MOTOR_THREE_QTR 191
#define MOTOR_FULL 255
#define ROT_SWITCH 19
#define SPEED_SWITCH 2

// tracking variables
int speedState = 0;
int fanDir = 0;
int fanSpeed = 0;
int fanInit = 0;
int buttonPress = 0;
int refresh = 0;
unsigned long lastPressDir = 0;
int dirPressed = 0;
int lastPressSpeed = 0;
int speedPressed = 0;
int cw = 1;
int ccw = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(45, 41, 33, 35, 37, 39);

//define a object of DS1307 class
DS1307 clock;

void setup() {

  Serial.begin(9600);
  // set pin direction
  pinMode(ROT_SWITCH, INPUT_PULLUP); // switch
  pinMode(SPEED_SWITCH, INPUT_PULLUP); // switch

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // set up RTC time
  clock.begin();
  clock.fillByYMD(2021,11,21);
  clock.fillByHMS(13,28,30);
  clock.fillDayOfWeek(SUN);
  clock.setTime();

  // attach an interrupt for the switch press
  attachInterrupt(digitalPinToInterrupt(ROT_SWITCH), changeDirection, LOW);
  attachInterrupt(digitalPinToInterrupt(SPEED_SWITCH), changeSpeed, LOW);

  // set pin direction
  pinMode(MOTOR_ENABLE,OUTPUT);
  pinMode(MOTOR_CW,OUTPUT);
  pinMode(MOTOR_CCW,OUTPUT);

  // turns on fan
  digitalWrite(MOTOR_CW, cw);
  digitalWrite(MOTOR_CCW, ccw);
  delay(100);
  //digitalWrite(MOTOR_ENABLE, HIGH);
  analogWrite(MOTOR_ENABLE, fanSpeed);  

}

void changeDirection() {
  if ((millis() - lastPressDir) > 100){

    lastPressDir = millis();
    dirPressed = 1;
    Serial.println("Button ISR Success");
  } 
  return;
}

void changeSpeed() {
  if ((millis() - lastPressSpeed) > 100){

    lastPressSpeed = millis();
    speedPressed = 1;
    Serial.println("Speed ISR Success");
  } 
  return;
}

void loop() {

  if(dirPressed){
    digitalWrite(MOTOR_ENABLE, LOW);
    delay(500);
    cw = !cw;
    ccw = !ccw;
    digitalWrite(MOTOR_CW, cw);
    digitalWrite(MOTOR_CCW, ccw);
    analogWrite(MOTOR_ENABLE, fanSpeed); 
    fanDir = !fanDir;   
    dirPressed = 0;
 }

  if(speedPressed){
      if(speedState ==3){
        speedState = 0;        
      } else {
        speedState++;
      }

  if(speedState == 0){
    fanSpeed = MOTOR_OFF;
  } else if(speedState == 1) {
    fanSpeed = MOTOR_HALF;
  } else if(speedState == 2){
    fanSpeed = MOTOR_THREE_QTR;
  } else if(speedState == 3){
    fanSpeed = MOTOR_FULL;
  }
  speedPressed = 0;
  analogWrite(MOTOR_ENABLE, fanSpeed); 
 }

  // clear lcd
  lcd.clear();
  
  // get time
  clock.getTime();
  
  // day of week
  switch (clock.dayOfWeek) // Friendly printout the weekday
    {
        case MON:
        lcd.print("MON");
        break;
        case TUE:
        lcd.print("TUE");
        break;
        case WED:
        lcd.print("WED");
        break;
        case THU:
        lcd.print("THU");
        break;
        case FRI:
        lcd.print("FRI");
        break;
        case SAT:
        lcd.print("SAT");
        break;
        case SUN:
        lcd.print("SUN");
        break;
    }
  lcd.print(" ");
  lcd.print(clock.hour, DEC);
  lcd.print(":");
  lcd.print(clock.minute, DEC);
  lcd.print(":");
  lcd.print(clock.second, DEC);

  // next line
  lcd.setCursor(0, 1);

  // direction
  if (fanDir == 0) {
    lcd.print("CW");
  }
  else {
    lcd.print("CCW");
  }

  // space
  lcd.print("   ");

  // speed
  if (fanSpeed == MOTOR_OFF) {
    lcd.print("0");
  }
  else if (fanSpeed == MOTOR_HALF) {
    lcd.print("1/2");
  }
  else if (fanSpeed == MOTOR_THREE_QTR) {
    lcd.print("3/4");
  }
  else {
    lcd.print("FULL");
  }
    
  // delay for debounce
  delay(1000);
}
