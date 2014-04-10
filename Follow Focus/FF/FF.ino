 Adi Soffer

 4.6.12

 */

#include <Servo.h>

#include <Bounce.h>

Servo myservo;

unsigned int potpin = 0;

unsigned int val;

uint8_t waitForServo = 5;           //delay time:let servo get to position 

uint8_t lastPosition;

uint8_t L;                               //a buffer int for updating lastPosition

uint8_t currentVal;

uint8_t x = 0;                         //counter to stop Rec.

//Defining Modes

int mode;

#define FREE 1

#define RECORD 2

#define PLAYBACK 3

#define STOP 4

#define REWIND 5

#define RECORDCONT 6

#define UPDATE 7

#define IO 8

#define REWINDIO 9

#define STOPIO 10

#define UPDATEIO 11

//Variables for Record and Playback

unsigned long inTime;                       //variable to hold time when recording starts           

unsigned long outTime;                     //variable to hold time when recording ends        

unsigned long deltaTime;                   //var holding actual recording time

uint8_t inPos;                                 //var to hold “in” value

uint8_t outPos;                               //var to hold “out” value

uint8_t deltaPos;                             //var to claculate num of steps

unsigned long servoTime;                  //var to claculate time for step in playback

//Blink without delay 

int ledState = LOW;

long previousMillis = 0;

long ledInterval = 75;

//Defining Switches

#define fBUTTON 7      //Free Mode Button

#define rBUTTON 8      //Record Button

#define hBUTTON 9      //IO Button

#define pBUTTON 10     //Play Button

//Defining LEDs

#define rLED 3     //Record LED

#define hLED 4     // IO LED

#define pLED 5     //Play LED

#define fLED 6     // Free Mode LED

//initial button states

int fbuttonValue = LOW;  //Free button Value

int rbuttonValue = LOW;  //Record button value

int hbuttonValue = LOW;  //IO button value

int pbuttonValue = LOW;  //Play button value

//Defining Bounce objects

Bounce fbouncer = Bounce(fBUTTON, 10);   //Free Mode Bouncer

Bounce rbouncer = Bounce(rBUTTON, 10);   //Record Bouncer

Bounce hbouncer = Bounce(hBUTTON, 10);   //IO Bouncer

Bounce pbouncer = Bounce(pBUTTON, 10);   //Play Bouncer

void setup() {

  Serial.begin(9600);

  Serial.flush();

  myservo.attach (2);

  pinMode(fBUTTON,INPUT);

  pinMode(rBUTTON,INPUT);

  pinMode(hBUTTON, INPUT);

  pinMode(pBUTTON, INPUT);

  pinMode(pLED, OUTPUT);

  pinMode(hLED, OUTPUT);

  pinMode(fLED,OUTPUT);

  pinMode(rLED,OUTPUT);

}

void loop() {

  //Record and Play Calculated

  deltaPos = abs (outPos - inPos);        

  deltaTime = (outTime - inTime);    

  servoTime = (deltaTime/deltaPos);

  if ( fbouncer.update() )                  //if FREE Button pressed

  {             

    if ( fbouncer.read() == HIGH) {

      if ( fbuttonValue == LOW ) {

        fbuttonValue = HIGH;

        mode = FREE;

      } 

      else {

        fbuttonValue = LOW;

        mode = STOP;

      }

    }

  }

  else if ( rbouncer.update() ) {              //if REC Button pressed

    if ( rbouncer.read() == HIGH) {

      if ( rbuttonValue == LOW ) {

        rbuttonValue = HIGH;

        mode = RECORD;

      } 

      else {

        rbuttonValue = LOW;

        mode = STOP;

      }

    }

  }

  else if ( hbouncer.update() ) {                //if REVERSE Button pressed

    if ( hbouncer.read() == HIGH) {

      if ( hbuttonValue == LOW ) {

        hbuttonValue = HIGH;

        mode = IO;

      } 

      else {

        hbuttonValue = LOW;

        mode = STOP;

      }

    }

  }

  else if ( pbouncer.update() ) {                 //if PLAY Button pressed

    if ( pbouncer.read() == HIGH) {

      if ( pbuttonValue == LOW ) {

        pbuttonValue = HIGH;

        mode = PLAYBACK;

      } 

      else {

        pbuttonValue = LOW;

        mode = STOP;

      }

    }

  }

  /*

«««««««DEFINING CASES»»»»»»»»

   */

  switch (mode){

  case FREE:                                     //«<FREE Focus Pulling»>//

    if (hbuttonValue == HIGH)                 //if in IO Mode

    {

      digitalWrite(fLED, HIGH);

      val = analogRead (potpin);

      val = map(val, 0, 1023, 0, 179);           

      inPos = val;

      lastPosition = val;

      delay (500);

      fbuttonValue = LOW;

      digitalWrite(fLED,LOW);

      mode = IO;

      break;

    }

    else                                          //if not in IO Mode

    {

      digitalWrite(fLED, HIGH);

      readWriteServo ();

      lastPosition = val;                 

      delay (waitForServo);                       

    }

    break;

  case RECORD:                           //«<RECORD Focus Pulling»>//

    if (hbuttonValue == HIGH)               //if in IO Mode

    {

      digitalWrite (rLED, HIGH);

      val = analogRead(potpin);           

      val = map(val, 0, 1023, 0, 179);   

      outPos = val;

      lastPosition = val;

      delay (500);

      rbuttonValue = LOW;

      digitalWrite (rLED, LOW);

      mode = IO;

      break;

    }

    else                                         //If in REC Mode

    {

      fbuttonValue = LOW;               

      digitalWrite(fLED, LOW);

      blinkFunction (3);

      readWriteServo ();

      /* Automatic REC triggering*/

      if (val > (lastPosition+2)|| val < (lastPosition - 2))  

      {

        inPos = lastPosition;                 //save IN pos 

        inTime = millis ( );                    //save IN time 

        mode = RECORDCONT;              //initiate REC

      }

    }

    break;

  case RECORDCONT:                         //«<RECORDING INITIATES»>//

    digitalWrite (hLED, LOW);             

    digitalWrite (rLED, HIGH);             

    readWriteServo ();

    outPos = val;                          //save OUT pos

    outTime = millis ( );                  //save OUT time

    lastPosition = val;

    delay(waitForServo);              

    /* Automatic REC terminating */

    if (abs (lastPosition - currentVal)>1)

    {

      currentVal = lastPosition; 

      x = 0;

    }

    else 

    {

      x = x+1;

      if (x == 30)

      {

        x = 0;

        rbuttonValue = LOW;

        mode = STOP;

      } 

    }

    break;

  case STOP:

    digitalWrite (rLED, LOW);

    digitalWrite (fLED, LOW);

    digitalWrite (pLED, LOW);

    digitalWrite (hLED, LOW);

    break;

  case REWIND: 

    fbuttonValue = LOW;                

    digitalWrite(fLED, LOW);

    pbuttonValue = LOW;                 

    digitalWrite(pLED, LOW);

    if (outPos == lastPosition)

    {

      if (outPos > inPos) 

      {  

        for (int i=outPos;i>(inPos-1);i—)

        {

          blinkFunction (5);

          rewind (i);

        }

        hbuttonValue = LOW;

        mode = STOP;

        break;

      }

      else if (outPos<inPos)

      {

        for (int i=outPos;i<(inPos+1);i++)

        { 

          blinkFunction (5);

          rewind (i);

        }

        hbuttonValue = LOW;

        mode = STOP;

        break;

      }

    }

    else if (inPos == lastPosition)

    {

      blinkFunction (5);

      mode = STOP;

      break;

    }

    else if (lastPosition > inPos) 

    {

      for (int i=lastPosition;i>(inPos-1);i—)

      {

        blinkFunction (5);

        myservo.write (i);

        delay (20);

        L = i;                                    //buffer int for updating lastPosition

      }

      hbuttonValue = LOW;

      mode = UPDATE;

      break;

    }

    else if (lastPosition < inPos)      

    {

      for (int i=lastPosition;i<(inPos+1);i++)

      {

        blinkFunction (5);

        myservo.write (i);

        delay (20);

        L = i;                                  //buffer int for updating lastPosition

      }

      hbuttonValue = LOW;

      mode = UPDATE;

      break;

    }

  case REWINDIO:

    if (lastPosition > inPos) 

    {  

      for (int i=lastPosition;i>(inPos-1);i—)

      {

        blinkFunction (5);

        myservo.write (i);

        delay (20);

        L = i;

      }

      mode = UPDATEIO;

      break;

    }

    else if (lastPosition<inPos)

    {

      for (int i=lastPosition;i<(inPos+1);i++)

      { 

        blinkFunction (5); 

        myservo.write (i);

        delay (20);

        L = i;

      }

      mode = UPDATEIO;

      break;

    }

  case UPDATEIO:

    lastPosition = L;

    mode = STOPIO;

    break;

  case UPDATE:                             //made to update 2 instances of REWIND

    lastPosition = L;

    mode = STOP;

    break;

  case PLAYBACK:

    digitalWrite(pLED, HIGH);

    if (hbuttonValue == HIGH)               //if in IO Mode

    {

      if (lastPosition != inPos)

      { 

        mode = REWINDIO;

        break;

      }    

      else if (outPos>inPos)

      {

        for (int i=inPos; i<(outPos+1); i++)  

        {

         playbackIO  (i);

        }

        pbuttonValue = LOW;

        digitalWrite (pLED, LOW);

        mode = STOPIO;

        break;

      }

      else if (outPos<inPos)

      {

        for (int i=inPos; i>(outPos-1); i—)  

        { 

         playbackIO  (i);

        }

        pbuttonValue = LOW;

        digitalWrite (pLED, LOW);

        mode = STOPIO;

        break;

      }

    }

    else                                           //if in REC Mode

    {

      digitalWrite (pLED, HIGH);

      if (lastPosition != inPos)

      {

        mode = REWIND;

        break;

      }

      else if (outPos>inPos)

      {

        for (int i=inPos; i<(outPos+1); i++)  

        {

          myservo.write (i);

          lastPosition = i;

          delay (servoTime);

        }

        pbuttonValue = LOW;

        digitalWrite (pLED, LOW);

        mode = STOP;

        break;

      }

      else if (outPos<inPos)

      {

        for (int i=inPos; i>(outPos-1); i—)  

        {

          myservo.write (i);

          lastPosition = i;

          delay (servoTime);

        }

        pbuttonValue = LOW;

        digitalWrite (pLED, LOW);

        mode = STOP;

        break;

      }

    }

  case IO:

    fbuttonValue = LOW;

    digitalWrite(fLED, LOW);   

    digitalWrite(hLED, HIGH);

    readWriteServo ();

    lastPosition = val;

    delay (waitForServo);                       

    break;

  case STOPIO:

    digitalWrite (rLED, LOW);

    digitalWrite (fLED, LOW);

    digitalWrite (pLED, LOW);

    pbuttonValue = LOW;

    break;

  }

}

//Functions

int blinkFunction (int y)

{

  // Blink without delay   

  unsigned long currentMillis = millis ();

  if (currentMillis - previousMillis>ledInterval)

  {

    previousMillis = currentMillis;

    if (ledState == LOW)

      ledState = HIGH;

    else

      ledState = LOW;

    digitalWrite (y, ledState);

  }

}

void readWriteServo ()

{

  val = analogRead(potpin);              

  val = map(val, 0, 1023, 0, 179);     

  myservo.write(val);                   

}

int rewind (int i)

{

  myservo.write (i);

  delay (20);

  lastPosition = i; 

}

int playbackIO (int i)

{

   int x = analogRead(potpin);                 // reads the value of the potentiometer  

          x = map(x, 0, 1023, 50, 1);         //Pot value sets playback speed 

          myservo.write (i);

          lastPosition = i;

          delay (x);                              //Pot value sets playback speed 

}

==========================================================
