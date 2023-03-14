/******************************
 * Author  Katja Schultz
 * eMail: schultzkatja@gmx.de
 * Date: 2023-01-03

 Description: This code reads 
 in the spot and average 
 temperature(of 10 values) of 
 an DS18B20 sensor connected 
 to an Arduino and saves it to
 an SD card  
  */

#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <RTClib.h>

RTC_DS1307 rtc; // to communicate with the clock

OneWire ow(4); // hand over pin to one wire bus
const String logfile = "tsensor.log";

#include "temp_logger_commands.h"

void setup() {
  Serial.begin(9600);

  // initialize and check if modules running
  if(!rtc.begin()) {
    Serial.println("RTC is NOT running. Let's set the time now!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // manually set date and time in case the arduino power is low and it has lost
  // the fllowing line sets the RTC to the data & time this sketch was compiled (!)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
  if(!SD.begin(10)) {
    Serial.println("SD module initialization failed or card is not present.");
    return; 
  }
  String header = "# timestamp, sensor_id, spot temperature in °C, average temperature in °C";
  printOutputln(header);
}

void loop() {
  byte rom_code[8]; // creates an array containing 8 elements of type byte for the ROM code
  byte sp_data[9]; // creates an array for reading in the data from the scratchpad
  String registration_number;

  float tempAverage = 0; // initialize average temperature to 0
  float tempSpot = 0; // initialize spot temperature to 0
  int numReadings = 10; // number of readings to take for averaging
  
  // start sequence to read out the rom code 
  ow.reset();
  ow.write(READ_ROM);
  for (int i=0; i<8; i++){
    rom_code[i] = ow.read();
  }
  if(rom_code[0] != IS_DS18B20_sensor){
    Serial.println("Sensor is not a DS18B20 sensor!");
  }
  
  for (int i=1; i<7; i++){
    registration_number += String(rom_code[i], HEX);
  }

// take multiple readings and calculate average
 
  for (int i=0; i<numReadings; i++) {
    // first sequence for converting temperature
    ow.reset();
    ow.write(SKIP_ROM);
    ow.write(CONVERT_T);
    
    delay(750); // wait for conversion to complete (750ms for 12-bit resolution)

    // second sequence for reading data from scratchpad
    ow.reset();
    ow.write(SKIP_ROM);
    ow.write(READ_SCRATCHPAD);

    for (int i=0; i<9; i++){
      sp_data[i] = ow.read();
    }
    int16_t tempRead = sp_data[1] << 8 | sp_data[0];

    float temp = tempRead / 16.0; // divide by 2^4 = 16 for 4 digits after the comma 
    tempAverage += temp; // add to temperature sum
    if (i==4){
      tempSpot = temp;
    }
  }
  
  tempAverage /= numReadings; // calculate average temperature

  printOutput(getISOtime());
  printOutput(",");
  printOutput(registration_number);
  printOutput(",");
  printOutput(String(tempSpot));
  printOutput(",");
  printOutputln(String(tempAverage));
  
}
