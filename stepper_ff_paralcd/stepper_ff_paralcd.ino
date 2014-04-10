// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

OneButton button(4, true); // Setup a OneButton on pin 4.
// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)

int encoderPin1 = 3;
int encoderPin2 = 2;
volatile int lastEncoded = 0;
int encoderValue;
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;
int lens;
int infinity =0 ;
int macro = 999;
int hardstop = 0;
int mark1;
int mark2;
int mark3;
int dist = 0;
int bat = 6;
char buffer [16];     //used with sprintinf to format text for LCD
int n;                //used with sprintinf to format text for LCD
long previoushardstop;

void setup() {
 
  Serial.println("Stepper test!");
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

button.attachDoubleClick(infinityFunction); // link the recordFunction to be called on a doubleclick event.     
button.attachClick (macroFunction);    // link the reverseFunction to be called on a click (short click) event.   
  button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event.
lcd.begin(16, 2); 
lcd.clear();
}

void loop() {
  button.tick(); //this checks for a button press event
 
  lcd.setCursor(0, 0);
  lcd.print("STP MAC INF BAT");
  lcd.setCursor(0, 1);
 n=sprintf (buffer, "%03d %03d %03d %02d ",hardstop, macro, infinity, bat);
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
  
  
  


  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

 
 
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)hardstop ++;

 

  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)hardstop --;

  lastEncoded = encoded; //store this value for next time

 hardstop = constrain(hardstop,infinity,macro);
 if (hardstop < previoushardstop)  {
   
previoushardstop = hardstop;
  } 

   if (hardstop > previoushardstop)  {
   
previoushardstop = hardstop;
   }
  }   
   

   void macroFunction(){
     macro = hardstop;
}
void infinityFunction(){
     infinity = hardstop;
}

   void resetFunction(){
     macro = 1000;
     infinity = 0;
}

