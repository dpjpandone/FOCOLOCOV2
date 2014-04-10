/*
  Calibration
 
 Demonstrates one technique for calibrating sensor input.  The
 sensor readings during the first five seconds of the sketch
 execution define the minimum and maximum of expected values
 attached to the sensor pin.
 
 The sensor minimum and maximum initial values may seem backwards.
 Initially, you set the minimum high and listen for anything 
 lower, saving it as the new minimum. Likewise, you set the
 maximum low and listen for anything higher as the new maximum.
 
 The circuit:
 * Analog sensor (potentiometer will do) attached to analog input 0
 * LED attached from digital pin 9 to ground
 
 created 29 Oct 2008
 By David A Mellis
 modified 30 Aug 2011
 By Tom Igoe
 
 http://arduino.cc/en/Tutorial/Calibration
 
 This example code is in the public domain.
 
 */

// These constants won't change:
const int sensorPin = A0;    // pin that the sensor is attached to
const int ledPin = 9;        // pin that the LED is attached to

// variables:
int calPos = 0;         // the sensor value
int calMin = 0;        // minimum sensor value
int calMax = 9999;           // maximum sensor value


void setup() {
  // turn on LED to signal the start of the calibration period:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // calibrate during the first five seconds 
  while (millis() < 5000) {
    calPos = analogRead(sensorPin);

    // record the maximum sensor value
    if (calPos > calMax) {
      calMax = calPos;
    }

    // record the minimum sensor value
    if (calPos < calMin) {
      calMin = calPos;
    }
  }

  // signal the end of the calibration period
  digitalWrite(13, LOW);
}

void loop() {
  // read the sensor:
  calPos = analogRead(sensorPin);



  // in case the sensor value is outside the range seen during calibration
  calPos = constrain(calPos, calMin, calMax);


}
