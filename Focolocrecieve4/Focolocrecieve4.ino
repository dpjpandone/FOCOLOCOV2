
#include <SPI.h>
#include <RFM69.h>


#include <Servo.h>

#include <AccelStepper.h>
// Define a stepper and the pins it will use
AccelStepper stepper(1, 5, 4);



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
int previouspos = 0;
bool promiscuousMode = true;                        //set to 'true' to sniff all packets on the same network



typedef struct 
{		
  int           pos;                                 //designate transmitted data for this potentiometer 0 value
} 
Payload;
Payload theData;



void setup() 
{


//stepper.setSpeed(200);
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(1000); 


  // Radio initialization
  Serial.begin(SERIAL_BAUD);
  delay(100);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);
digitalWrite(6,LOW);//to disable microstep 1
digitalWrite(7,LOW); //to disable microstep 2
}



void loop() 
{ 


  if (radio.receiveDone()) 
  {  

    theData = *(Payload*)radio.DATA;                       //assume radio.DATA actually contains our struct and not something else

stepper.moveTo(theData.pos);   

 }
//stepper.setSpeed(1000);
  //stepper.runSpeedToPosition();
  stepper.run();
  }

