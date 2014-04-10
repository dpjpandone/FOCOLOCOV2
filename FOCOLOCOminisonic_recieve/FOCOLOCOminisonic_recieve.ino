
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <SerialLCD.h>
#include <Wire.h>
#include <Encoder.h>

#include <JeeLib.h>
#include <RF12sio.h>

RF12 RF12;






Encoder myEnc(3, 4);
OneButton button(5, true); // Setup a OneButton on pin 4.

int infinity =0 ;
int astop = 0;
int macro = 999;
int bstop = 999;

int reso = 4;
int hardstop = 0;
int softstop = 0;
int dist = 0;
int bat;
char buffer [16];     //used with sprintinf to format text for LCD
int n;                //used with sprintinf to format text for LCD
int m;
long oldPosition  = -999;
long previoushardstop = 0;

// Constructor.  Parameters: rows, columns, baud/i2c_address, interface (RS232, I2C, SPI)
SerialLCD lcd(2,16,0x28,I2C);

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
byte inf[8] = {
        B11111,
        B11111,
        B11111,
        B01110,
        B11111,
        B01110,
        B01110,
        B01110
};

byte mac[8] = {
        B10101,
        B11111,
        B11111,
        B01110,
        B10101,
        B01110,
        B00100,
        B00100
};

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = (1126400L / result)*1.2; // Back-calculate AVcc in mV
  return result;
}
void setup() {
  
Serial.begin(9600);
    Serial.print("\n[rf12serial]");
    rf12_config();
    
  // Initialize LCD module
  lcd.init();
  lcd.clear();

  // create a new character
  lcd.createChar(0, dead);
  lcd.createChar(1, low);
  lcd.createChar(2, good);
  lcd.createChar(3, full);
  lcd.createChar(4, inf);
  lcd.createChar(5, mac);
  
  



}

void loop() {
  if (infinity < 1)  {
button.attachClick (infinityFunction);    // link the reverseFunction to be called on a click (short click) event.   

 } 
   else {
    button.attachClick (astopFunction);    // link the reverseFunction to be called on a click (short click) event. 
 
} 
    
    if (macro > 998)  {
button.attachDoubleClick(macroFunction); // link the recordFunction to be called on a doubleclick event.    

 } 
else {
    button.attachDoubleClick(bstopFunction); // link the recordFunction to be called on a doubleclick event. 

}
         
     
  button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event. 
  

  
  
  long newPosition = myEnc.read();

  if (oldPosition < newPosition)  {

    hardstop--;
    softstop--;
    oldPosition = newPosition;  
} 

  if (oldPosition > newPosition)  {

    hardstop++;
    softstop++;
    oldPosition = newPosition;  
}

   if ((infinity < astop) && (macro > bstop))  {
hardstop = constrain(hardstop,astop,bstop);    

 }
else {
    hardstop = constrain(hardstop,infinity,macro); 

} 

  
  if (hardstop != previoushardstop) {

 lcd.setCursor(1,15);
 lcd.print("TX");
 lcd.setCursor(1,15);
 lcd.print("  ");
     
  
        dist = RF12.read(); 

    previoushardstop = hardstop;
    
    lcd.setCursor(1,15);
    lcd.print(dist);

  }

  button.tick(); //this checks for a button press event

   m=sprintf (buffer, "%03d POS B:%03d",macro,bstop);
   lcd.setCursor(1, 1);
   lcd.write(byte(5));
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
  
  
  n=sprintf (buffer, "%03d %03d A:%03d",infinity,hardstop,astop);
  lcd.setCursor(2, 1);
  lcd.write(byte(4));
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
  

  bat = ( readVcc() );

  lcd.setCursor(2,16);

if (bat <4800) {
  lcd.write(byte(0));
  }
if ((bat>= 4801) && (bat <=5000))   {
  lcd.write(byte(1));
  }
if ((bat>= 5001) && (bat <=5604))   {
  lcd.write(byte(2));
  }
if (bat >=5605) {
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
  astop = 0;
  bstop = 999;
  
}
void astopFunction(){
  astop = hardstop;
}

void bstopFunction(){
bstop = hardstop;
}



