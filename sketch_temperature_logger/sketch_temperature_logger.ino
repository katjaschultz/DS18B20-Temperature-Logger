/********************
 * Author  Katja Schultz
 * eMail: schultzkatja@gmx.de
 * Date: 2023-01-03
  */

#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <RTClib.h>

RTC_DS1307 rtc; // to communicate with the clock

OneWire ow(4); // hand over pin to one wire bus

void setup() {
  Serial.begin(9600);

  // initialize and check if modules running
  if(!rtc.begin()) {
    Serial.println("RTC is NOT running. Let's set the time now!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
