#include <Wire.h>
#include <SoftwareSerial.h>
char Buttons[] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};
char pressedButton;               //contains the Caracter of the last pressed button
boolean pressed;                  //went true if a button is pressed
unsigned long lastmillis = 0;              //counter ageinst bouncing and rapid repeating
unsigned long millisDelay = 0;             //not used variable(?)
unsigned long lastpress = 0;
unsigned long xDelayTime, yDelayTime;

//pins for Led Matrix:
const short dataIn = 4;                   //pins used for the LED Matrix
const short load = 5;
const short clock = 6;

const short sender = 8;
const short emfaenger = 7;
SoftwareSerial mySerial(emfaenger, sender);

const int switchPin = 9;      // switch to turn on and off mouse control
const int mouseButton = 9;    // input pin for the mouse pushButton
const int xAxis = A2;         // joystick X axis
const int yAxis = A3;         // joystick Y axis
int range = 150;               // output range of X or Y movement
int responseDelay = 3;        // response delay of the mouse, in ms
float threshold = range / 128;    // resting threshold
int center = range / 2;       // resting position value

byte max7219_reg_noop        = 0x00;
byte max7219_reg_digit0      = 0x01;
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

int maxInUse = 1;

int extenderwrite = 0x38;
int extenderread = 0x38;
unsigned long nextpress;

int currentXPos = 0;
int currentYPos = 0;

byte arrayToSend[6][6] = {0};
byte arrayToDisplay[8] = {0};

void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // get bitmask
    digitalWrite( clock, LOW);   // tick
    if (data & mask){            // choose bit
      digitalWrite(dataIn, HIGH);// send 1
    }else{
      digitalWrite(dataIn, LOW); // send 0
    }
    digitalWrite(clock, HIGH);   // tock
    --i;                         // move to lesser bit
  }
}

void maxAll (byte reg, byte col) {    // initialize  all  MAX7219's in the system
  int c = 0;
  digitalWrite(load, LOW);  // begin    
  for ( c =1; c<= maxInUse; c++) {
  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
    }
  digitalWrite(load, LOW);
  digitalWrite(load,HIGH);
}

void maxSingle( byte reg, byte col) {    
//maxSingle is the "easy"  function to use for a single max7219
 
  digitalWrite(load, LOW);       // begin    
  putByte(reg);                  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data  
  digitalWrite(load, LOW);       // and load da stuff
  digitalWrite(load,HIGH);
}

void setup() {
  mySerial.begin(4800);
  Serial.begin(115200);
    Wire.begin();                   //for the expander that controlls the buttons
  pinMode(dataIn, OUTPUT);        //the LED Matrix pinMode-setup
  pinMode(clock,  OUTPUT);        //
  pinMode(load,   OUTPUT);        //
  pinMode(9, INPUT_PULLUP);       // the switch pin joystick
  pinMode(A2, INPUT);             //Joystick x Achse
  pinMode(A3, INPUT);             //Joystick y Aches
  maxAll(max7219_reg_scanLimit, 0x07);   //Matrix setup
  maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxAll(max7219_reg_displayTest, 0x00); // no display test
   for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x0f & 0x0f);    
}

void joystick(){
  int xValue = map(analogRead(A2),3,1020,-2,2); if(xValue<0)xValue++; if(xValue>0)xValue--;
  int yValue = map(analogRead(A3),0,1020,-2,2); if(yValue<0)yValue++; if(yValue>0)yValue--;
  if(xDelayTime < millis()){
  switch(xValue){
    case(1):
        currentXPos--;
        if(currentXPos == -1) currentXPos = 5;
        xDelayTime = millis() + 200;
    break;
    case(-1):
        currentXPos++;
        if(currentXPos == 6) currentXPos = 0;
        xDelayTime = millis() + 200;
    break;
    default:
      if(digitalRead(9)==LOW){
        sendToCube();
        xDelayTime = millis() + 300;
      }

  }
  }
  if(yDelayTime < millis()){
    switch(yValue){
      case(1):
          currentYPos--;
          if(currentYPos == -1) currentYPos = 5;
          yDelayTime = millis() + 200;
      break;
      case(-1):
          currentYPos++;
          if(currentYPos == 6) currentYPos = 0;
          yDelayTime = millis() + 200;
      break;
      default:
        yDelayTime = millis()+50;
    }
}

}

void getbutton(){
int pressCounter;
  
  for(int i=0;i<=3;i++){
  Wire.beginTransmission(extenderwrite); //Öffnen der Verbindung
  Wire.write(1<<i);
  Wire.endTransmission();
  Wire.requestFrom(extenderread,1,true);
  if(!Wire.read() && nextpress < millis()){
    for(int j=4;j<=7;j++){
      Wire.beginTransmission(extenderwrite); //Öffnen der Verbindung
      Wire.write(1<<j);
      Wire.endTransmission();
      Wire.requestFrom(extenderread, 1,1);
      if(!Wire.read()){
        pressedButton = Buttons[(i*4)+(j-4)];
        nextpress = millis() + 300;
        pressCounter++;
      } //end if col
    } //end for col (j)
  }//end if row
}   //end for row (i)
 (pressCounter)? pressed=true : pressed=false;
pressCounter = 0;

} //end getbutton()

void sendToCube(){ 
 // for(short x=0;x<6;x++){
 //   for(short y=0;y<6;y++){
      mySerial.write(currentXPos);
      mySerial.write(currentYPos);
      mySerial.write(arrayToSend[currentXPos][currentYPos]);
      delay(50);
 //   }
 // }
}

void setBytes(){
  switch(pressedButton){
    case '1':
      arrayToSend[currentXPos][currentYPos] ^= 32;
    break;
    case '2':
      arrayToSend[currentXPos][currentYPos] ^= 16;
    break;
    case '3':
      arrayToSend[currentXPos][currentYPos] ^= 8;
    break;
    case '4':
      arrayToSend[currentXPos][currentYPos] ^= 4;
    break;
    case '5':
      arrayToSend[currentXPos][currentYPos] ^= 2;
    break;
    case '6':
      arrayToSend[currentXPos][currentYPos] ^= 1;
    break;
    case '7':
      arrayToSend[currentXPos][currentYPos] = B111111;
    break;
    case '9':
      arrayToSend[currentXPos][currentYPos] = 0;
    break;
    case '#':
      sendToCube();
    break;
  }
}

void showDisplay(){
 for(short i=0;i<8;i++) arrayToDisplay[i] &= B0; //clears all arrays
 arrayToDisplay[currentYPos+2] |= B10000000;            //shows the layer
 arrayToDisplay[0] = 1<<currentXPos;             //shows the active row
 for(short x=0;x<6;x++){
  for(short y=0;y<6;y++){
    bitWrite(arrayToDisplay[x+2], y, bitRead(arrayToSend[y][currentYPos], x));
  }
 }
 for(short i=0;i<8;i++)maxSingle(i+1,arrayToDisplay[i]);
}

void loop() {
  getbutton();                    //test if something is pressed
  joystick();
  if(pressed)setBytes();
  showDisplay();
}
