#define SRCLR 12 //pin 10 //reg-rst //active low
#define SRCLK 11  //pin 11 //clock
#define RCLK 10   //pin 12 //latch
#define OE 9    //pin 13 //active low
#define SER 8   //pin 14 //data
#define LAYER1 A0
#define LAYER2 A1
#define LAYER3 A2
#define LAYER4 5
#define LAYER5 6
#define LAYER6 7


byte Daten1 = B00000000;
//byte Daten2 = B00011110;
//byte Daten3 = B00111110;
byte Daten4 = B00011111;
byte Daten5 = B00011110;
//byte Daten6 = B00010101;

byte outputArray[6][6]={0};

byte testbyte[6][6] = {
  {Daten1, Daten5, Daten5, Daten5, Daten5, Daten1}, 
  {Daten1, Daten4, Daten4, Daten4, Daten4, Daten5}, 
  {Daten1, Daten4, Daten4, Daten4, Daten4, Daten5}, 
  {Daten1, Daten4, Daten4, Daten4, Daten4, Daten5}, 
  {Daten1, Daten4, Daten4, Daten4, Daten4, Daten5}, 
  {Daten1, Daten1, Daten1, Daten1, Daten1, Daten1}
};
                      

void out(byte input[6][6]){
  digitalWrite(SRCLR, LOW); digitalWrite(SRCLR, HIGH); //resets all strorrage register
  for(int i=0;i<6;i++){
    digitalWrite(RCLK, LOW);
    for(int j=0;j<6;j++){
      shiftOut(SER, SRCLK, MSBFIRST, input[i][j]);
    }
    digitalWrite(RCLK, HIGH);
    switch(i){
      case 0:
        digitalWrite(LAYER1, LOW); delay(2); digitalWrite(LAYER1, HIGH);
      break;
      case 1:
        digitalWrite(LAYER2, LOW); delay(2); digitalWrite(LAYER2, HIGH);
      break;
      case 2:
        digitalWrite(LAYER3, LOW); delay(2); digitalWrite(LAYER3, HIGH);
      break;
      case 3:
        digitalWrite(LAYER4, LOW); delay(2); digitalWrite(LAYER4, HIGH);
      break;
      case 4:
        digitalWrite(LAYER5, LOW); delay(2); digitalWrite(LAYER5, HIGH);
      break;
      case 5:
        digitalWrite(LAYER6, LOW); delay(2); digitalWrite(LAYER6, HIGH);
      break;
    }
  }
}

void setup() {
DDRB = DDRB | B111111; //sets digitalpins 8-13 as outputs
DDRD = DDRD | B11100000; //sets digitalpin 2-7 as outputs, ignores 0-1
DDRC = DDRC | B00000111; //sets analogpin A0-A2 as outputs, ignores the rest
digitalWrite(SRCLR, HIGH); //disables register Reset
analogWrite(OE, 0); //sets Brightness
}

void generateRandom(){
  
}

void staticOne(){
  out(testbyte);
}

void loop() {
analogWrite(OE, 0);
//out(testbyte);
staticOne();

}
