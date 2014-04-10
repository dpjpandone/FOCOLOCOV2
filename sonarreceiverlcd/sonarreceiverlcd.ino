#include <SPI.h>
#include "Ultrasonic.h"
#include <nRF24L01.h>
#include <RF24.h>
Ultrasonic ultrasonic(7,6);
#define CE_PIN   9
#define CSN_PIN 10
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
int joystick[1];  // 1 element array holding Joystick readings

void setup() {
Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);

}

void loop()
{

  Serial.print(ultrasonic.Ranging(CM));
  joystick[0] = ultrasonic.Ranging(CM);
  
  
  
    
  radio.write( joystick, sizeof(joystick) );
    
  delay(100);
}




