
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

// Constructor.  Parameters: rows, columns, baud/i2c_address, interface (RS232, I2C, SPI)
SerialLCD lcd(2,16,0x28,I2C);


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

  // Initialize LCD module
  lcd.init();
  lcd.clear();
delay(100);

  

  
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
lcd.print("INFINITY: ");
lcd.print(infinity);
lcd.print("       ");

  


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



  m=sprintf (buffer, "B:%04d POS:     ",bstop);
  lcd.setCursor(1, 1);
  
  lcd.print(buffer); ///< You will get “000 00 00 00 00″


  n=sprintf (buffer, "A:%04d %04d      ",astop,hardstop);
  lcd.setCursor(2, 1);
 
  lcd.print(buffer); ///< You will get “000 00 00 00 00″


 bat = ( readVcc() );


if ((bat>= 3700) && (bat <=3799))   {
  lcd.setCursor(1,16);
  lcd.print("CHANGE BATTERY");
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
  bstop = 9999;

}
void astopFunction(){
  astop = hardstop;
}

void bstopFunction(){
  bstop = hardstop;
}





