#include "ibus.h"
#include <AltSoftSerial.h>


#define UPDATE_INTERVAL 5 // milliseconds
#define BAUD_RATE 115200
#define ANALOG_REFERENCE DEFAULT

// values for receving from bluetooth
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;



// 1. Analog channels. Data can be read with the Arduino's 10-bit ADC.
#define ANALOG_INPUTS_COUNT 8
byte analogPins[] = {A0, A1, A2, A3, A4, A5, A6, A7}; // element count MUST be == ANALOG_INPUTS_COUNT


// 2. Digital channels. Data can be read from Arduino's digital pins.
#define DIGITAL_INPUTS_COUNT 9
byte digitalPins[] = {2, 3, 4, 5, 6, 7, 10, 11, 12}; // element count MUST be == DIGITAL_INPUTS_COUNT


//***********************************************
#define NUM_CHANNELS ( (ANALOG_INPUTS_COUNT) + (DIGITAL_INPUTS_COUNT))
/*
analog pin 8, digital pin 9
first digital pin is 9th on recever
*/
//***********************************************

IBus ibus(NUM_CHANNELS);
// for bluetooth device
AltSoftSerial blueSerial; //8->Rx, 9->Tx


void setup()
{
  analogReference(ANALOG_REFERENCE); // use the defined ADC reference voltage source
  Serial.begin(BAUD_RATE);           // setup serial
  blueSerial.begin(9600);
  for(int i=0; i<DIGITAL_INPUTS_COUNT; i++){
    pinMode(digitalPins[i], INPUT_PULLUP);
  }
}


int wheelDegree;
void loop()
{
  int i, bm_ch = 0;
  unsigned long time = millis();

  ibus.begin();

  // update to read data from bluetooth
  recvWithEndMarker();

  // get wheel degree if avalible
  if(newData = true) {
    wheelDegree = atoi(receivedChars);
    newData = false;
  }
  
  // read analog pins - one per channel
  for(i=0; i < ANALOG_INPUTS_COUNT; i++)
    if(i == 1){
      ibus.write(wheelDegree);
    }else{
      ibus.write(analogRead(analogPins[i])); 
    }

  // read digital pins - one per channel
  for(i=0; i < DIGITAL_INPUTS_COUNT; i++)
    ibus.write(digitalRead(digitalPins[i]) == HIGH ? 1023 : 0);

  
  ibus.end();

  time = millis() - time; // time elapsed in reading the inputs
  if(time < UPDATE_INTERVAL)
    // sleep till it is time for the next update
    delay(UPDATE_INTERVAL  - time);
}


// receive data from Ev3 via bluetooth device
void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (blueSerial.available() > 0 && newData == false) {
        rc = blueSerial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}
