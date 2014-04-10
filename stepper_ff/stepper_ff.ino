// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <AFMotor.h>
#include <SerialLCD.h>
#include <Wire.h>
#include <SPI_VFD.h>
OneButton button(9, true); // Setup a OneButton on pin 4.
// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(200, 2);
int encoderPin1 = 3;
int encoderPin2 = 2;
volatile int lastEncoded = 0;
int encoderValue;
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;
int lens;
int infinity =0 ;
int macro = 1000;
int hardstop = 0;
int mark1;
int mark2;
int mark3;

SerialLCD lcd(2,16,9600,RS232);
long previoushardstop;
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  motor.release();
  motor.setSpeed(100);  // 10 rpm
button.attachDoubleClick(infinityFunction); // link the recordFunction to be called on a doubleclick event.     
button.attachClick (macroFunction);    // link the reverseFunction to be called on a click (short click) event.   
  button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event.
lcd.init();  

}

void loop() {
  button.tick(); //this checks for a button press event
  
  lcd.println(hardstop);
  
  
  


  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

 
 
 
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)hardstop ++;

 

  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)hardstop --;

  lastEncoded = encoded; //store this value for next time

 hardstop = constrain(hardstop,infinity,macro);
 if (hardstop < previoushardstop)  {
   motor.step(1, BACKWARD, DOUBLE);
previoushardstop = hardstop;
  } 

   if (hardstop > previoushardstop)  {
   motor.step(1, FORWARD, DOUBLE);
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

