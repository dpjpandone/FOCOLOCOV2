// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(A0,A1);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
Encoder myEnc(2, 3);
OneButton button(4, true); // Setup a OneButton on pin 4.
// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)


int lens;
int infinity =1 ;
int macro = 999;
int hardstop = 1;
int mark1;
int mark2;
int mark3;
int dist = 0;
int bat = 6;
char buffer [16];     //used with sprintinf to format text for LCD
int n;                //used with sprintinf to format text for LCD

long oldPosition  = -999;

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe



int joystick[2];  // 2 element array holding Joystick readings

void setup() {
 
 


button.attachDoubleClick(infinityFunction); // link the recordFunction to be called on a doubleclick event.     
button.attachClick (macroFunction);    // link the reverseFunction to be called on a click (short click) event.   
  button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event.
lcd.begin(16, 2); 
lcd.clear();
  

  radio.begin();
  radio.openWritingPipe(pipe);
}

void loop() {
 
   lcd.setCursor(0, 0);
  lcd.print("STP MAC INF BAT");
  
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    
  }

hardstop = constrain(hardstop,infinity,macro);
 if (oldPosition < newPosition)  {
   radio.write( joystick, sizeof(joystick) ); 
oldPosition = newPosition;
  } 

   if (oldPosition > newPosition)  {
    radio.write( joystick, sizeof(joystick) );
oldPosition = newPosition;
   }
  
 
  lcd.setCursor(0, 1);
 n=sprintf (buffer, "%03d %03d %03d %02d ",hardstop, macro, infinity, bat);
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
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

