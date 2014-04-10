/*
 * MCP7940RTC.h - library for DS1307 RTC
  
  Copyright (c) AnalyticTools LLC
  This library uses Arduino Time.h library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  10 Jan 2012 - Initial release
 */
#include <Time.h>
#include <Wire.h>
#include "MCP7940RTC.h"
typedef uint8_t byte;

#define MCP7940_CTRL_ID 0x6F 
byte b[32];

MCP7940RTC::MCP7940RTC()
{
  Wire.begin();

  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100    
  Wire.write((byte)0x00);
#else
  Wire.send(0x00);
#endif
  Wire.endTransmission();
  // Check status of oscillator, start it if not already running.
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100    
  b[0] = Wire.read();
#else
  b[0] = Wire.receive();
#endif
  if((b[0] & 0x80)==0) {      // Start oscillator
    // Start the oscillator
    //Serial.println("starting osc.");
    Wire.beginTransmission(MCP7940_CTRL_ID);
    b[1] = b[0] | 0x80;
    b[0] = 0;
#if ARDUINO >= 100    
    Wire.write(&b[0],2);
#else
	Wire.send(&b[0],2);
#endif
    Wire.endTransmission();
  }

  // Check status of VBAT, and ensure disabled
  // Check the status of VBAT, and disable it
  Wire.beginTransmission(MCP7940_CTRL_ID);
  Wire.write((byte)0x03);
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100    
  b[0] = Wire.read();
#else
  b[0] = Wire.receive();
#endif
  if((b[0] & 0x08)>0) {
    //Disable the VBAT mode
    //Serial.println("disabling vbat");
    Wire.beginTransmission(MCP7940_CTRL_ID);
    b[1] = b[0] |= 0x08;
    b[0] = 3;
#if ARDUINO >= 100    
    Wire.write(&b[0],2);
#else
	Wire.send(&b[0],2);
#endif
    Wire.endTransmission();
  }
}
  
// PUBLIC FUNCTIONS
time_t MCP7940RTC::get()   // Aquire data from buffer and convert to time_t
{
  tmElements_t tm;
  read(tm);
  return(makeTime(tm));
}

void  MCP7940RTC::set(time_t t)
{
  tmElements_t tm;
  breakTime(t, tm);
  tm.Second &= 0x7f;  // stop the oscillator and write the data
  write(tm);
  uint8_t s = (tm.Second | 0x80); // assert oscillator start bit
  // Start the oscillator
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00);			
  Wire.write((dec2bcd(tm.Second) | 0x80));							// Seconds
#else
  Wire.send(0x00);
  Wire.send(s);
#endif
  Wire.endTransmission();
}

void MCP7940RTC::clearAlarm0() {
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x07);
  Wire.write((uint8_t)0);
#else
  Wire.send(0x07);
  Wire.send(0);
#endif
  Wire.endTransmission();

}

void MCP7940RTC::setAlarm0(time_t t)
{
  tmElements_t tm;
  breakTime(t, tm);
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x0a);			
  Wire.write((uint8_t)dec2bcd(tm.Second) & 0x7f);		// Seconds
  Wire.write((uint8_t)dec2bcd(tm.Minute) & 0x7f);		// Minutes
  Wire.write((uint8_t)dec2bcd(tm.Hour) & 0x3f);			// Hour
  Wire.write((uint8_t)(dec2bcd(tm.Wday) | 0x70) & 0x97);// wDay, trigger on minutes matching
  Wire.write((uint8_t)dec2bcd(tm.Day) & 0x3f);			// Day
  Wire.write((uint8_t)dec2bcd(tm.Month) & 0x1f);		// Hour
#else
  Wire.send(0x0a);
  Wire.send(dec2bcd(tm.Second & 0x7f));			// Seconds
  Wire.send(dec2bcd(tm.Minute & 0x7f));			// Minutes
  Wire.send(dec2bcd(tm.Hour & 0x3f));			// Hour
  Wire.send((dec2bcd(tm.Wday) | 0x70) & 0xf7);	// wDay
  Wire.send(dec2bcd(tm.Day) & 0x3f);			// Day
  Wire.send(dec2bcd(tm.Month) & 0x1f);			// Hour
#endif
  Wire.endTransmission();

// enable alarm 0
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x07);
  Wire.write((uint8_t)0x10);
#else
  Wire.send(0x07);
  Wire.send(0x10);
#endif
  Wire.endTransmission();
}


// Aquire data from the RTC chip in BCD format
void MCP7940RTC::read(tmElements_t &tm)
{
  uint8_t d[8];
  uint8_t dmask[] = { 0x7f, 0x7f, 0x3f, 0x07, 0x3f, 0x1f, 0xff };
  memset(d,0,8);

  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); 
#else
  Wire.send(0x00);
#endif  
  Wire.endTransmission();

  // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
  Wire.requestFrom(MCP7940_CTRL_ID, tmNbrFields);
#if ARDUINO >= 100    
  tm.Second = bcd2dec((Wire.read() & 0x7f));   
  tm.Minute = bcd2dec((Wire.read() & 0x7f));
  byte b1 = Wire.read();
  byte b2 = b1 & 0x1f;
  if((b1 & 0x40)>0) 
	b2 |= 0x20;
  tm.Hour = bcd2dec(b1 & 0x3f);
  //tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec((Wire.read() & 0x07));
  tm.Day = bcd2dec(Wire.read() & 0x3f );
  tm.Month = bcd2dec(Wire.read() & 0x1f);
  //tm.Year = bcd2dec(Wire.read()) + 30;//
  tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
#else
  tm.Second = bcd2dec(Wire.receive() & 0x7f);   
  tm.Minute = bcd2dec(Wire.receive() );
  byte b1 = Wire.receive();
  byte b2 = b1 & 0x1f;
  if((b1 & 0x40)>0) 
	b2 |= 0x20;
  tm.Hour =   bcd2dec(b1 & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec(Wire.receive() );
  tm.Day = bcd2dec(Wire.receive() & 0x07);
  tm.Month = bcd2dec(Wire.receive() & 0x1f );
  tm.Year = y2kYearToTm((bcd2dec(Wire.receive())));
#endif  
}

uint8_t MCP7940RTC::getSecond() {
 byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)0);
#else
  Wire.send(0x00);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif
  b = bcd2dec(b1 & 0x7f);
  //b = (b1 & 0xf) + ((b1 & 0x70)>>4) * 10;
  return b;
}

uint8_t MCP7940RTC::getMinute() {
 byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)1);
#else
  Wire.send(1);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif
  b = bcd2dec(b1 & 0x7f);
  //b = (b1 & 0xf) + ((b1 & 0x70)>>4) * 10;
  return b;
}

uint8_t MCP7940RTC::getHour() {
  //TODO: Check for 24-hr formation vs. am/pm, but for now, treat as 24-hr format due to init setting of same
  byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)2);
#else
  Wire.send(2);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif;
  byte b2 = b1 & 0x1f;
  if((b1 & 0x40)>0) b2 |= 0x20;
  b = bcd2dec(b1 & 0x3f);
  return b;
}

uint8_t MCP7940RTC::getDayOfWeek() {
  byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)3);
#else
  Wire.send(3);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif;
  byte b2 = b1 & 0x07;
  return b;
}

uint8_t MCP7940RTC::getDay() {
  byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)4);
#else
  Wire.send(4);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif;
  b = bcd2dec(b1 & 0x3f);
  return b;
}

uint8_t MCP7940RTC::getMonth() {
  byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)5);
#else
  Wire.send(5);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif;
  b = bcd2dec(b1 & 0x1f);
  return b;
}

uint8_t MCP7940RTC::getYear() {
  byte b=0;
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((byte)6);
#else
  Wire.send(6);
#endif
  Wire.endTransmission();
  Wire.requestFrom(MCP7940_CTRL_ID, 1);
#if ARDUINO >= 100
  byte b1 = Wire.read();
#else
  byte b1 = Wire.receive();
#endif;
  return bcd2dec(b1);
}

void MCP7940RTC::write(tmElements_t &tm)
{
  Wire.beginTransmission(MCP7940_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); // reset register pointer  
  Wire.write((uint8_t)dec2bcd(tm.Second) & 0x7f) ;   // Seconds
  Wire.write((uint8_t)dec2bcd(tm.Minute) & 0x7f);    // Minutes
  Wire.write((uint8_t)dec2bcd(tm.Hour) & 0x3f);     // sets 24 hour format
  Wire.write((uint8_t)dec2bcd(tm.Wday) & 0x07);   
  Wire.write((uint8_t)dec2bcd(tm.Day) & 0x3f);
  Wire.write((uint8_t)dec2bcd(tm.Month) & 0x1f);
  Wire.write((uint8_t)dec2bcd(tm.Year));//tmYearToY2k(tm.Year))); 
#else  
  Wire.send(0x00); // reset register pointer  
  Wire.send(dec2bcd(tm.Second & 0x7f)) ;   
  Wire.send(dec2bcd(tm.Minute & 0x7f));
  Wire.send(dec2bcd(tm.Hour & 0x3f));      // sets 24 hour format
  Wire.send(dec2bcd(tm.Wday & 0x07));   
  Wire.send(dec2bcd(tm.Day & 0x3f));
  Wire.send(dec2bcd(tm.Month & 0x1f));
  Wire.send(dec2bcd(tm.Year));//tmYearToY2k(tm.Year)));   
#endif
  Wire.endTransmission();  
}
// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t MCP7940RTC::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t MCP7940RTC::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

//MCP7940RTC RTC = MCP7940RTC(); // create an instance for the user

