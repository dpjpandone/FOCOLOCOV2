
#include <RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>


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
   int           Pot_0;                              //create transmit variable & store data potentiometer 0 data
 } Payload;
 Payload theData;                                    // create transmission package 'theData'.

void setup() 
{
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);                             // Output to serial operating frequency of transceiver       
  

}

void loop() 
{
 // Output to serial some communications information - Data length, RSSI
    for (byte i = 0; i < radio.DATALEN; i++)
    Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.readRSSI());Serial.print("]");
    Serial.println();

 // translate Pot rotation to Servo position.    
    theData.Pot_0 = map(analogRead(A0), 0, 1023, 0, 179);                                              // assign servo position to transmit packet variable.
  
 // Transmit payload - theData
    radio.send(GATEWAYID, (const void*)(&theData), sizeof(theData));   // transmit data to other end
       
 // Output to serial information of # bytes sent.
    Serial.print("Sending struct (");
    Serial.print(sizeof(theData));
    Serial.println(" bytes) ");
    Serial.println();

    Blink(LED,3);                                                      
    // Board indicator of radio transmitting/receiving.
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
