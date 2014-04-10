
#include <SPI.h>
#include "OneButton.h" //this library allows buttons to call mutiple events: http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
#include <SerialLCD.h>
#include <Wire.h>
#include <Encoder.h>
#include <RFM69.h>
#include <RFM69registers.h>

#define ENCODER_USE_INTERRUPTS
#define ENCODER_OPTIMIZE_INTERRUPTS

#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 9600
//#define ACK_TIME    30  // # of ms to wait for an ack ---- Not Currently Used.

// Ack will not be used. This is a streaming application. If a packet is lost the next servo position data
// will be accurate enough to update and move the servo.


RFM69 radio;                                         // initiate radio object
typedef struct 
 {		
   int           Pot_0, dist;                              //create transmit variable & store data potentiometer 0 data
 } Payload;
 Payload theData;                                    // create transmission package 'theData'.


Encoder myEnc(3, 4);
OneButton button(5, true); // Setup a OneButton on pin 4.

int infinity =0 ;
int astop = 0;
int macro = 1023;
int bstop = 1023;

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

void setup() {
  pinMode(6, OUTPUT);
  digitalWrite(6,LOW);

     Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
    
  // Initialize LCD module
  lcd.init();
  lcd.clear();


}

void loop() {

button.attachClick (astopFunction);    // link the reverseFunction to be called on a click (short click) event. 
button.attachDoubleClick(bstopFunction); // link the recordFunction to be called on a doubleclick event. 
//button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event. 
button.attachPress (calibrateFunction);   // link the playbackFunction to be called on a press (long click) event.   
  
 

 hardstop = analogRead(0);  


     hardstop = map(hardstop, 0,1023,astop,bstop);    



  
if (previoushardstop != hardstop ) {
  
// translate Pot rotation to Servo position.    
    theData.Pot_0 = hardstop;                                              // assign servo position to transmit packet variable.
  
 // Transmit payload - theData
    radio.send(GATEWAYID, (const void*)(&theData), sizeof(theData));   // transmit data to other end
       

Blink();
   
previoushardstop=hardstop;
}
  

  button.tick(); //this checks for a button press event

   m=sprintf (buffer, "POS: B:%04d",bstop);
   lcd.setCursor(1, 1);
  
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
  
  
  n=sprintf (buffer, "%04d A:%03d",hardstop,astop);
  lcd.setCursor(2, 1);
  
  lcd.print(buffer); ///< You will get “000 00 00 00 00″
  




}


void macroFunction(){
  macro = hardstop;
}
void infinityFunction(){
  infinity = hardstop;
}

void resetFunction(){
  astop = infinity;
  bstop = macro;
}
void rcalibrateFunction(){
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

  

  
 if (previoushardstop != hardstop ) {
  
// translate Pot rotation to Servo position.    
    theData.Pot_0 = hardstop;                                              // assign servo position to transmit packet variable.
  
 // Transmit payload - theData
    radio.send(GATEWAYID, (const void*)(&theData), sizeof(theData));   // transmit data to other end
       


   
previoushardstop=hardstop;
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
  
  button.attachClick (infinityFunction);    // link the reverseFunction to be called on a click (short click) event.  
button.attachDoubleClick(macroFunction); // link the recordFunction to be called on a doubleclick event. 
button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event.
}
void astopFunction(){
  astop = hardstop;
}

void bstopFunction(){
bstop = hardstop;
}

void Blink()
{
  
  digitalWrite(6,HIGH);
  delay(1);
  digitalWrite(6,LOW);
}

