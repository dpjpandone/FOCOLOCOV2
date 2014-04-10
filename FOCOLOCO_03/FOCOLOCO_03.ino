
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
RF24 radio(A0,A1); // Set up nRF24L01 radio on SPI bus plus pins A0 & A1




LiquidCrystal lcd(12, 11, 10, 9, 8, 7);// initialize the library with the numbers of the interface pins
Encoder myEnc(2, 3);
OneButton button(4, true); // Setup a OneButton on pin 4.

int infinity =0 ;
int macro = 999;
int hardstop = 0;
int dist = 0;
int bat;
char buffer [16];     //used with sprintinf to format text for LCD
int n;                //used with sprintinf to format text for LCD
long oldPosition  = -999;
long previoushardstop = 0;

byte dead[8] = {
 0b01110,
	0b11011,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b11111
};
byte low[8] = {
       	0b01110,
	0b11011,
	0b10001,
	0b10001,
	0b10001,
	0b11111,
	0b11111,
	0b11111
};
byte good[8] = {
      0b01110,
	0b11011,
	0b10001,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};
byte full[8] = {
     0b01110,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111,
	0b11111
};

void setup() {

  // create a new character
  lcd.createChar(0, dead);
  lcd.createChar(1, low);
  lcd.createChar(2, good);
  lcd.createChar(3, full);

  button.attachDoubleClick(infinityFunction); // link the recordFunction to be called on a doubleclick event.     
  button.attachClick (macroFunction);    // link the reverseFunction to be called on a click (short click) event.   
  button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event.

  lcd.begin(16, 2); 
  lcd.clear();
}

void loop() {

  long newPosition = myEnc.read();

  if (oldPosition < newPosition)  {

    hardstop++;
    oldPosition = newPosition;  

  } 

  if (oldPosition > newPosition)  {

    hardstop--;
    oldPosition = newPosition;  

  }

  hardstop = constrain(hardstop,infinity,macro);

  if (hardstop != previoushardstop) {


    //insert radio send here

    previoushardstop = hardstop;

  }

  button.tick(); //this checks for a button press event

  lcd.setCursor(0, 0);
  lcd.print("STP MAC INF");
  lcd.setCursor(0, 1);
  n=sprintf (buffer, "%03d %03d %03d",hardstop, macro, infinity);
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
  bat = analogRead(A5);
  bat = map(bat, 0,988, 0,6);
  lcd.setCursor(15,0 );
  
if (bat <5) {
  lcd.write(byte(0));
}
if (bat = 5 ) {
  lcd.write(byte(2));
}
if (bat >5) {
  lcd.write(byte(3));
}
}


void macroFunction(){
  macro = hardstop;
}
void infinityFunction(){
  infinity = hardstop;
}

void resetFunction(){
  macro = 999;
  infinity = 0;
}



