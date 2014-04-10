
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
OneButton button(5, true); // Setup a OneButton on pin 4.

int infinity =0 ;
int astop = 0;
int macro = 2000;
int bstop = 2000;

int reso = 4;
int hardstop = 0;
int softstop = 0;
int fade;
int bat;
char buffer [16];     //used with sprintinf to format text for LCD
int n;                //used with sprintinf to format text for LCD
int m;
long oldPosition  = -999;
long previoushardstop = 0;
const int numReadings = 2333333333333333333333333333333333330;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0; 
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
  pinMode(6, OUTPUT);
  digitalWrite(6,LOW);

  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);

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


  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;  


}

void loop() {
  if (infinity < 1)  {
    button.attachClick (infinityFunction);    // link the reverseFunction to be called on a click (short click) event.   

  } 
  else {
    button.attachClick (astopFunction);    // link the reverseFunction to be called on a click (short click) event. 

  } 

  if (macro > 1999)  {
    button.attachDoubleClick(macroFunction); // link the recordFunction to be called on a doubleclick event.    

  } 
  else {
    button.attachDoubleClick(bstopFunction); // link the recordFunction to be called on a doubleclick event. 

  }


  button.attachPress (resetFunction);   // link the playbackFunction to be called on a press (long click) event. 





 
    // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(A0); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;         
  // send it to the computer as ASCII digits 
  
  hardstop=average;
  
  if ((infinity < astop) && (macro > bstop))  {

    hardstop = map(hardstop, 0,1023,astop,bstop);    

  }
  else {

    hardstop = map(hardstop,0,1023,infinity,macro); 



  if (hardstop != previoushardstop) {

    theData.pos = hardstop;                                              // assign servo position to transmit packet variable.
    radio.send(GATEWAYID, (const void*)(&theData), sizeof(theData));   // transmit data to other end
    previoushardstop=hardstop;
Blink();  

  


  
  button.tick(); //this checks for a button press event

int infinitydisp,macrodisp, bstopdisp,astopdisp,hardstopdisp;
infinitydisp = map (infinity,0,2000,0,999);
macrodisp = map (macro,0,2000,0,999);
bstopdisp = map (bstop,0,2000,0,999);
astopdisp = map (astop,0,2000,0,999);
hardstopdisp = map (hardstop,0,2000,0,999);

  m=sprintf (buffer, "%03d POS B:%03d",macrodisp, bstopdisp);
  lcd.setCursor(1, 1);
  lcd.write(byte(5));
  lcd.print(buffer); ///< You will get “000 00 00 00 00″


  n=sprintf (buffer, "%03d %04d A:%03d",infinitydisp,hardstop,astopdisp);
  lcd.setCursor(2, 1);
  lcd.write(byte(4));
  lcd.print(buffer); ///< You will get “000 00 00 00 00″


 bat = ( readVcc() );


if (bat <3699) {
  lcd.setCursor(1,16);
  lcd.write(byte(0));
  }
if ((bat>= 3700) && (bat <=3799))   {
  lcd.setCursor(1,16);
  lcd.write(byte(1));
  }
if ((bat>= 3800) && (bat <=3929))   {
  lcd.setCursor(1,16);
  lcd.write(byte(2));
  }
if (bat >=3930) {
  lcd.setCursor(1,16);
  lcd.write(byte(3));

  } 
if (hardstop != infinity && hardstop != macro && hardstop != astop && hardstop != bstop){
digitalWrite(6,LOW);
}
else{
  digitalWrite(6,HIGH);
  }

}} }



void macroFunction(){
  macro = hardstop;
}
void infinityFunction(){
  infinity = hardstop;
}

void resetFunction(){
  astop = 0;
  bstop = 2000;

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
delayMicroseconds(100);
  digitalWrite(6,LOW);
}



