#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define BACKLIGHT_PIN 13
LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

volatile byte xPos, yPos, input = 0;
byte counter = 0;

void setup() {
Serial.begin(4800);
pinMode(A0,INPUT);
pinMode(BACKLIGHT_PIN, OUTPUT);
digitalWrite(BACKLIGHT_PIN, HIGH);
lcd.begin(16, 2);
lcd.backlight();
}

void serialEvent(){     //if serial data is available
while(Serial.available()>0){                        //falls der eingansbuffer nicht leer
  byte  incomingByte = Serial.read();                     //lese erstes byte, lösche es aus dem buffer
    switch (counter){
      case 0:
        xPos = incomingByte;
        counter++;
      break;
      case 1:
        yPos = incomingByte;
        counter++;
      break;
      case 2:
        input = incomingByte;
        counter = 0;
      break;
      default:
        counter = 0;
      break;
    }
  }
  lcd.clear();
}

void loop() {
lcd.setCursor(0,0);
lcd.print(xPos);
lcd.setCursor(7,0);
lcd.print(yPos);
lcd.setCursor(0,1);
for(byte i=5;i>0;i--)if(input<(1<<i))lcd.print("0");
lcd.print(input, BIN);
delay(50);

}
