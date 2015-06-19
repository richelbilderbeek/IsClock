/*
  IsClock
  (C) 2015 Richel Bilderbeek

2015-06-14: v.1.0: Initial version, shows hours divided by minutes
2015-06-19: v.1.1: Added showing minutes divided by seconds

*/

//if NDEBUG is #defined, this is a release version
#define NDEBUG

#include <LiquidCrystal.h>
#include <Time.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int error_pin = 13;

//Date of release
const int release_day   =   19;
const int release_month =    6;
const int release_year  = 2015;
 
void OnError(const String& error_message)
{
  Serial.print("ERROR: ");  
  Serial.println(error_message);  
  while (1)
  {
    //Blink LED
    digitalWrite(error_pin,!digitalRead(error_pin));
    //Write to serial
    delay(1000);
  }
}

void SetTimeFromSerial()
{
  //Serial.println("Start of SetTimeFromSerial");
  const int  h  = Serial.available() ? Serial.parseInt() : -1;
  delay(10);
  const char c1 = Serial.available() ? Serial.read() : '0';
  delay(10);
  const int  m  = Serial.available() ? Serial.parseInt() : -1;
  delay(10);
  const char c2 = Serial.available() ? Serial.read() : '0';
  delay(10);
  const int  s  = Serial.available() ? Serial.parseInt() : -1;
  delay(10);
  const String used = String(h) + String(c1) + String(m) + String(c2) + String(s);
  if (h == -1) 
  {
    Serial.println(String("No hours, use e.g. '12:34:56' (used '") + used + String("')"));
    return;
  }
  if (c1 == '0') 
  {
    Serial.println("No first seperator, use e.g. '12:34:56' (used '" + used + "')");
    return;
  }
  if (m == -1) 
  {
    Serial.println("No minutes, use e.g. '12:34:56' (used '" + used + "')");
    return;
  }
  if (c2 == '0') 
  {
    Serial.println("No second seperator, use e.g. '12:34:56'");
    return;
  }
  if (s == -1) 
  {
    Serial.println("No seconds, use e.g. '12:34:56'");
    return;
  }
  if (h < 0 || h > 23)
  {
    Serial.println("Hours must be in range [0,23]");
    return;
  }
  if (m < 0 || h > 59)
  {
    Serial.println("Minutes must be in range [0,59]");
    return;
  }
  if (s < 0 || s > 59)
  {
    Serial.println("Seconds must be in range [0,59]");
    return;
  }
  setTime(h,m,s,release_day,release_month,release_year); 
}


void setup()
{
  pinMode(error_pin, OUTPUT);  
  Serial.begin(9600); //Cannot be used: chip is used stand-alone
  #ifndef NDEBUG
  Serial.println("IsClock v. 1.0 (debug version)");
  #else //NDEBUG
  Serial.println("IsClock v. 1.0 (release version)");
  #endif //NDEBUG
  //Set the date to the release data, otherwise the clock may lag to underflow values
  setTime(0,0,0,release_day,release_month,release_year); 
}

void loop()
{
  while (1)
  {
    if (Serial.available())
    {
      delay(100);
      SetTimeFromSerial();  
    }
    
    //Show the time
    const int s = second();
    const int m = minute();
    const int h = hour();
    
    ShowTime(s,m,h);

    //Show common time in serial monitor
    { 
      const String time_now = String(h) + ":" + String(m) + ":" + String(s);
      Serial.println(time_now);
      delay(100);
    }
  }
}

void ShowTime(const int secs, const int mins, const int hours)
{
  #ifndef NDEBUG
  if (hours <  0) { OnError("ShowTime: hours <  0, hours = " + String(hours)); }
  if (hours > 23) { OnError("ShowTime: hours > 23, hours = " + String(hours)); }
  if (mins <  0) { OnError("ShowTime: mins <  0, mins = " + String(mins)); }
  if (mins > 59) { OnError("ShowTime: mins > 59, mins = " + String(mins)); }
  if (secs <  0) { OnError("ShowTime: secs <  0, secs = " + String(secs)); }
  if (secs > 59) { OnError("ShowTime: secs > 59, secs = " + String(secs)); }
  #endif // NDEBUG
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println(static_cast<double>(hours) / static_cast<double>(mins));
  lcd.setCursor(0,1);
  lcd.println(static_cast<double>(mins) / static_cast<double>(secs));
}
