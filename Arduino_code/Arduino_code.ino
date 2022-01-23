#include "ibus.h"
#include <AltSoftSerial.h>


// //////////////////
// Edit here to customize

// How often to send data?
#define UPDATE_INTERVAL 5 // milliseconds

// To disable reading a specific type of pin, set the count to 0 and remove all items from the pins list

// 1. Analog channels. Data can be read with the Arduino's 10-bit ADC.
// This gives values from 0 to 1023.
// Specify below the analog pin numbers (as for analogRead) you would like to use.
// Every analog input is sent as a single channel.
// Arduino Mega has 16 analog pins, however if your device has fewer you'll need to modify the count and pin list below

#define ANALOG_INPUTS_COUNT 8
byte analogPins[] = {A0, A1, A2, A3, A4, A5, A6, A7}; // element count MUST be == ANALOG_INPUTS_COUNT


// 2. Digital channels. Data can be read from Arduino's digital pins.
// They could be either LOW or HIGH.
// Specify below the digital pin numbers (as for digitalRead) you would like to use.
// Every pin is sent as a single channel. LOW is encoded as 0, HIGH - as 1023
// Arduino Mega has 54 digital only pins and the ability to read the analog pins as digital via pin numbers 55-68. If your device has fewer you'll need to modify the count and pin list below
#define DIGITAL_INPUTS_COUNT 9
byte digitalPins[] = {2, 3, 4, 5, 6, 7, 10, 11, 12}; // element count MUST be == DIGITAL_INPUTS_COUNT


// Define the appropriate analog reference source. See
// https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/
// Based on your device voltage, you may need to modify this definition
#define ANALOG_REFERENCE DEFAULT

// Define the baud rate
#define BAUD_RATE 115200

// /////////////////



//***********************************************
#define NUM_CHANNELS ( (ANALOG_INPUTS_COUNT) + (DIGITAL_INPUTS_COUNT))
/*
analog pin 8, digital pin 9
first digital pin is 9th on recever
*/
//***********************************************

IBus ibus(NUM_CHANNELS);
AltSoftSerial blueSerial;
//8->Rx, 9->Tx


void setup()
{
  analogReference(ANALOG_REFERENCE); // use the defined ADC reference voltage source
  Serial.begin(BAUD_RATE);           // setup serial
  blueSerial.begin(9600);
  for(int i=0; i<DIGITAL_INPUTS_COUNT; i++){
    pinMode(digitalPins[i], INPUT_PULLUP);
  }
}

void loop()
{
  int i, bm_ch = 0;
  unsigned long time = millis();

  ibus.begin();

  // read analog pins - one per channel
  for(i=0; i < ANALOG_INPUTS_COUNT; i++)
    ibus.write(analogRead(analogPins[i]));

  // read digital pins - one per channel
  for(i=0; i < DIGITAL_INPUTS_COUNT; i++)
    ibus.write(digitalRead(digitalPins[i]) == HIGH ? 1023 : 0);

  
  ibus.end();

  time = millis() - time; // time elapsed in reading the inputs
  if(time < UPDATE_INTERVAL)
    // sleep till it is time for the next update
    delay(UPDATE_INTERVAL  - time);
}
