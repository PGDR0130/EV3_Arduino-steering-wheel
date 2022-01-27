#include "ibus.h"
#include <AltSoftSerial.h>
#include <LiquidCrystal_I2C.h>


#define UPDATE_INTERVAL 5 // milliseconds
#define BAUD_RATE 115200
#define ANALOG_REFERENCE DEFAULT

// for every arudino environment
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif


// values for receving from bluetooth
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;

// 1. Analog channels. Data can be read with the Arduino's 10-bit ADC.
#define ANALOG_INPUTS_COUNT 6
byte analogPins[] = {A0, A1, A2, A3, A6, A7}; // element count MUST be == ANALOG_INPUTS_COUNT


// 2. Digital channels. Data can be read from Arduino's digital pins.
#define DIGITAL_INPUTS_COUNT 9
byte digitalPins[] = {2, 3, 4, 5, 6, 7, 10, 11, 12}; // element count MUST be == DIGITAL_INPUTS_COUNT


//***********************************************
#define NUM_CHANNELS ( (ANALOG_INPUTS_COUNT) + (DIGITAL_INPUTS_COUNT))
/*
analog pin 6, digital pin 9
first digital pin is 7th on recever
*/
//***********************************************

IBus ibus(NUM_CHANNELS);
// for bluetooth device
AltSoftSerial blueSerial; //8->Rx, 9->Tx
LiquidCrystal_I2C lcd(0x27, 20, 4); // uses A4 and A5 for I2C connections


void setup()
{
  analogReference(ANALOG_REFERENCE); // use the defined ADC reference voltage source
  Serial.begin(BAUD_RATE);           // setup serial
  blueSerial.begin(9600);
  for(int i=0; i<DIGITAL_INPUTS_COUNT; i++){
    pinMode(digitalPins[i], INPUT_PULLUP);
  }

  // setup custom charactor for lcd display
  setupChar();

  // start lcd 
  lcd.init();
  lcd.backlight();
  setupChar();

  lcd.home();
}



int DC[8] = {1,1,1,1,1,1,1,1};
int wheelDegree;
float display_time = millis();
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
  for(i=0; i < ANALOG_INPUTS_COUNT; i++){
    if(i == 1){ // get wheel degree from bluetooth not pin
      ibus.write(wheelDegree);
    }else{
      ibus.write(analogRead(analogPins[i])); 
    }
  }
  // read digital pins - one per channel
  for(i=0; i < DIGITAL_INPUTS_COUNT; i++){
    int temp_D = digitalRead(digitalPins[i]);
    if(i < 8){
      DC[i] = temp_D;
    }
    ibus.write(temp_D == HIGH ? 1 : 0);
  }
  
  ibus.end();

  blueSerial.println("Out");
  // LCD display informations from DC
  if(millis() - display_time > 250){
    blueSerial.println("In");
    lcd.clear();
    lcd.setCursor(0, 0);

    lcdPrint();
//    for(int i=0; i<8; i++){
//      lcd.print(DC[i]);
//    }
    display_time = millis();    
  }

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

uint8_t L1[8] = {0x13,0x11,0x13,0x12,0x1B,0x00,0x15,0x0A};



// setup custom char for the lcd
void setupChar(){

  // pin 1 - 8, two by two
  byte oneTwo[8] = {0x02,0x06,0x02,0x07,0x1C,0x04,0x08,0x1C};
  byte threeFour[8] = {0x03,0x01,0x03,0x01,0x13,0x14,0x1C,0x04};
  byte fiveSix[8] = {0x07,0x04,0x07,0x19,0x13,0x1C,0x14,0x1C};
  byte sevenEight[8] = {0x07,0x05,0x01,0x1D,0x15,0x1C,0x14,0x1C};
  // indicator 
  byte o_up[8] = {0x07,0x07,0x07,0x07,0x07,0x00,0x00,0x00};
  byte t_up[8] = {0x00,0x00,0x00,0x1C,0x1C,0x1C,0x1C,0x1C};
  byte ot_up[8] = {0x07,0x07,0x07,0x1B,0x1B,0x1C,0x1C,0x1C};

  lcd.createChar(0, oneTwo);
  lcd.createChar(1, threeFour);
  lcd.createChar(2, fiveSix);
  lcd.createChar(3, sevenEight);
  lcd.createChar(4, o_up);
  lcd.createChar(5, t_up);
  lcd.createChar(6, ot_up);
  //lcd.createChar(7, );
  
}

// printing lcd display
void lcdPrint(){
  // print pins
  for(int i=0; i<4; i++){
    lcd.setCursor(2*i, 0);
    lcd.printByte(i);
  }
  // print state
  for(int i=0; i<4; i++){
    lcd.setCursor(2*i + 1, 0);
    
    if(DC[i*2] && DC[i*2 + 1]){
      lcd.printByte(6);
    }else if(DC[i*2]){
      lcd.printByte(4);
    }else if(DC[i*2+1]){
      lcd.printByte(5);
    }
  }
  // print analog
  lcd.setCursor(0, 1);
  lcd.print("degree: "); lcd.print(analogRead(A0));// any analog pin or number
}
