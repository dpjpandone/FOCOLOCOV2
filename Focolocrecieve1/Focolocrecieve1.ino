/* Wirelessly receive potentiometer signals to actuate a servo motor using an
    RFM69HW enabled Moteino connection
 
 Referenced & using example code, logic, format from:
 * https://github.com/LowPowerLab/RFM69/tree/master/Examples
   Lowpowerlab.com RFM69 examples: Node & Struct_send by Felix Rusu
 * http://www.instructables.com/id/Arduino-2-Servos-Thumbstick-joystick/step4/The-Code/
   by Biomech75 2012
 * http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
   code by David A. Mellis 2005; modified by Paul Stoffregen 8 Feb 2010
 * http://www.jeremyblum.com/2011/02/27/arduino-tutorial-9-wireless-communication/
   code by Jeremy Blum
 * http://forum.arduino.cc/index.php?PHPSESSID=jts7bh8gq4mos865mb4kbsi4l0&topic=151699.15
   code examples within forum string by 'ashh'
 * Credit for RFM12B code from Glyn Hudson openenergymonitor.org GNU GPL V3 12/4/12
 * Credit to JCW from Jeelabs.org for RFM12 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 
 * http://forum.arduino.cc/index.php/topic,154463.0.html

 This example code is in the public domain.

 B.Stott 2013
 */

 #include <RFM69.h>
 #include <SPI.h>

 #include <Servo.h>

 #define NODEID      1
 #define NETWORKID   100
 #define FREQUENCY   RF69_915MHZ                      //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
 #define KEY         "thisIsEncryptKey"               //has to be same 16 characters/bytes on all nodes, not more not less!
 #define LED         9
 #define SERIAL_BAUD 9600
 #define ACK_TIME    30                               // # of ms to wait for an ack --- Not currently used.

// Ack will not be used. This is a streaming application. If a packet is lost the next servo position data
// will be accurate enough to update and move the servo.

 RFM69 radio;                                         // initiate radio object
 
 bool promiscuousMode = false;                        //set to 'true' to sniff all packets on the same network

     //  Setup Servo Objects
 Servo Servo0;

 typedef struct 
 {		
  int           pos;                                 //designate transmitted data for this potentiometer 0 value
 } Payload;
 Payload theData;

 void setup() 
 {

 // Attach Servo Objects to respective PWM pins
  Servo0.attach(5); 

 // Radio initialization
  Serial.begin(SERIAL_BAUD);
  delay(100);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);

  }

 byte ackCount=0;

 void loop() 
 { 


  if (radio.receiveDone()) 
  {  

	
    if (radio.DATALEN != sizeof(Payload))
      Serial.print("Invalid payload received, not matching Payload struct!");
    else
    {
      theData = *(Payload*)radio.DATA;                       //assume radio.DATA actually contains our struct and not something else

   // Adjust RF data for Servo Output
      theData.pos = constrain(theData.pos, 0, 180);



      Servo0.write(theData.pos);                           // Write position to servo0
    }

  }}
