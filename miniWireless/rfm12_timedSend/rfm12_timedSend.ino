// Experiment with time-controlled periodic transmission.
// 2011-06-24 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
//
// 2013-12-15 Anarduino - modifications to cycle through a varying packet length
//            for purposes of visually seeing the receiver screen data
//
#include <JeeLib.h>

MilliTimer sendTimer;
byte pending;
word seqnum;

char dat[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
void setup () {
  Serial.begin(115200);
  Serial.println("\n[timedSend]");
  rf12_initialize(25, RF12_915MHZ, 4);
}

int cnt=0;
int n=2;
void loop () {
  if (rf12_recvDone() && rf12_crc == 0 && rf12_len == 2) {
    sendTimer.set(0);
    Serial.print(" #");
    Serial.print(seqnum);
    Serial.print(" start: ");
    Serial.print(rf12_data[0], DEC);
    Serial.print(" recvd: ");
    Serial.println(rf12_data[1], DEC);
  }
  
  if (sendTimer.poll(2000))
    pending = 1;
  
  if (pending && rf12_canSend()) {
    pending = 0;
    if(n>=sizeof(dat))
      n=2;
    rf12_sendStart(RF12_HDR_ACK, (const char *)&dat[0], n);//"hello!", 6);
    n++;
    ++seqnum;
  }
}
