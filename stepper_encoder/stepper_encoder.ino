// Use a Rotary encoder with the ADAfruit Motor shield and a stepper motor
//Jason Welsh 2012
#include <AFMotor.h>
int state, prevstate = 500, count = 500;
int nextEncoderState[4] = { 2, 0, 3, 1 };
int prevEncoderState[4] = { 1, 3, 0, 2 };



// Number of steps per turn of encoder
int forward=1;
int backward=1;

//Used to calculate direction
int direct=0;


// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(48, 2);


void setup() {
motor.setSpeed(100);  // 100 rpm 

//place a Rotary encoder on pins 
//A=Pin10, Ground=Pin9 B=Pin2

  pinMode(10, INPUT);
  pinMode(2, INPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  digitalWrite(2, HIGH);
  digitalWrite(10, HIGH);
  Serial.begin(115200); 

}

void loop() {
   
   state = (digitalRead(10) << 1) | digitalRead(2);
  if (state != prevstate) {
    if (state == nextEncoderState[prevstate]) {
       count++;
       direct= count-1;
       
      if (count > direct){
      
      motor.step(forward, FORWARD, SINGLE);
       
       
   Serial.println("________forward____________________");// used for testing purpose
   direct=0;  

  
   }
       
     
    } else if (state == prevEncoderState[prevstate]) {
       count--;
       direct= count+1;
       
        if (count < direct){
    
       motor.step(backward, BACKWARD, SINGLE);  
       
     
      
       Serial.println("__________backward_________________");// used for testing purpose
      direct=0; 
      
    }
    }
    Serial.print("count =");
    Serial.println(count, DEC);
    Serial.println(direct, DEC);
    Serial.print("prev count =");
    
  
    prevstate = state;
  
    }


}
