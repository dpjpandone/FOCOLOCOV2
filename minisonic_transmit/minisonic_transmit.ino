// Serial data over RF12 demo, works in both directions
// 2009-04-17 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
#include "Ultrasonic.h"

Ultrasonic ultrasonic(4,5);
#include <JeeLib.h>
#include <RF12sio.h>

RF12 RF12;

void setup() {
    Serial.begin(9600);
    Serial.print("\n[rf12serial]");
    rf12_config();
}

void loop() {
   
        RF12.send(ultrasonic.Ranging(CM));
    Serial.print(ultrasonic.Ranging(CM));
    delay(100);
}
