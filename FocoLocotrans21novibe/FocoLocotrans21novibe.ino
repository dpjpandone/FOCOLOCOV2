//Pin Usage:
 //Radio uses: D2 for interrupt, D10 for chip enable,  D11,D12,D13 for SPI
// RTC uses D3 for interrupt, A4,A5 for I2C
 //RTC I2C Address used for chip communications: 0x6F
 //128Mb Memory uses D5 for chip select, D11,D12,D13 for SPI
 //D9 is the LED

#include <SPI.h>
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <SerialLCD.h>
#include <Wire.h>
#include <Encoder.h>
#include <RFM69.h>
#include <RFM69registers.h>

#define ENCODER_USE_INTERRUPTS
#define ENCODER_OPTIMIZE_INTERRUPTS
#define INPUT_PULLUP;

#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 9600
//#define ACK_TIME    30  // # of ms to wait for an ack ---- Not Currently Used.

// Ack will not be used. This is a streaming application. If a packet is lost the next  position data
// will be accurate enough to update and move the servo.


RFM69 radio;                                         // initiate radio object




typedef struct 
{		
  int           pos;                              //create transmit variable & store data potentiometer 0 data
} 
Payload;
Payload theData;                                    // create transmission package 'theData'.





Encoder myEnc(3, 4);
Encoder menuEnc(6, 7);
OneButton button(5, true); // Setup a OneButton on pin 5.

int resolution = 1;
int infinity ;
int bstop = 9999;
int macro = 0;
int astop = 0;

int reso = 4;
int hardstop = 0;
int softstop = 0;

int bat;
char buffer [16];     //used with sprintinf to format text for LCD
int n;                //used with sprintinf to format text for LCD
int m;
long oldPosition  = 0;
long previoushardstop = 0;
int vibeState = LOW;             // ledState used to set the LED
long previousMillis = 0;

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
   Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
pinMode(9,OUTPUT);
  // Initialize LCD module
  lcd.init();
    // create a new character
  lcd.createChar(0, dead);
  lcd.createChar(1, low);
  lcd.createChar(2, good);
  lcd.createChar(3, full);
  lcd.createChar(4, inf);
  lcd.createChar(5, mac);
 
  lcd.clear();


  

  
  // calibrate during the first five seconds 
  while (digitalRead(5)==HIGH) { 

 
  lcd.setCursor(1, 1);
lcd.print("PUSH 2 CALIBRATE");

  





hardstop = myEnc.read();
        hardstop = constrain(hardstop,0,9999);
        if (hardstop != previoushardstop) {

    theData.pos = hardstop;                                              // assign servo position to transmit packet variable.
    radio.send(GATEWAYID, (const void*)(&theData), sizeof(theData));   // transmit data to other end
    previoushardstop=hardstop;
 
}
      
      infinity = hardstop;
    
 lcd.setCursor(2, 1);
lcd.print("  INFINITY: ");
lcd.print(infinity);
lcd.print("     ");

  


  }}





void loop() {

button.attachClick (bstopFunction);    // link the reverseFunction to be called on a click (short click) event. 
button.attachDoubleClick(astopFunction); // link the recordFunction to be called on a doubleclick event. 
button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event. 

resolution = menuEnc.read();
resolution = constrain(resolution,4,16);
 if (resolution <= 4) {

    resolution=4;
 
}

hardstop = myEnc.read();
hardstop = constrain(hardstop,macro,infinity); 



  if (hardstop != previoushardstop) {

    theData.pos = hardstop;                                              // assign servo position to transmit packet variable.
    radio.send(GATEWAYID, (const void*)(&theData), sizeof(theData));   // transmit data to other end
    previoushardstop=hardstop;
 
}


  button.tick(); //this checks for a button press event



  m=sprintf (buffer, "B:%04d  X%d     ",bstop,resolution/4);
  lcd.setCursor(1, 1);
  
  lcd.print(buffer); ///< You will get “000 00 00 00 00″


  n=sprintf (buffer, "A:%04d %04d",astop,hardstop);
  lcd.setCursor(2, 1);
 
  lcd.print(buffer); ///< You will get “000 00 00 00 00″


 bat = ( readVcc() );
 

if (bat <3950) {
  lcd.setCursor(1,16);
  lcd.write(byte(0));
  }
if ((bat>= 3950) && (bat <=3999))   {
  lcd.setCursor(1,16);
  lcd.write(byte(1));
  }
if ((bat>= 4000) && (bat <=4999))   {
  lcd.setCursor(1,16);
  lcd.write(byte(2));
  }
if (bat >=5000) {
  lcd.setCursor(1,16);
  lcd.write(byte(3));
}

 
 
  lcd.setCursor(2,13);

 if ((hardstop != bstop)&&(hardstop != astop))   {
 lcd.print("     ");
 }  
else {
    lcd.print("STOP"); 

}
 if ((hardstop != bstop-1)&&(hardstop != astop+1))   {
 digitalWrite (9,LOW);
 }  
else {
 


  // check to see if it's time to vibrate; that is, if the 
  // difference between the current time and last time you vibed 
  // is bigger than the interval at which you want to 
  // vibe.
 
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > 300) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

  
      digitalWrite(9, HIGH);
      delay(500);
    
      digitalWrite(9, LOW);


}}
}


void macroFunction(){
  macro = hardstop;
}
void infinityFunction(){
  infinity = hardstop;
}

void resetFunction(){
  astop = 0;
  bstop = 9999;

}
void astopFunction(){
  astop = hardstop;
}

void bstopFunction(){
  bstop = hardstop;
}





