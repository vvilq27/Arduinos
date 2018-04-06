// Transmitter usb0

#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


//--------------servo--------------

int analogPin = 0; 
int analogPin2 = 1;
int meas = 0;
int meas2 = 0;

//-------------nrf-----------------
int msg[1];
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;


void setup(void){
   Serial.begin(9600);
   radio.begin();
   radio.openWritingPipe(pipe);
}

void loop(void){
    //meas = analogRead(analogPin);
    meas2 = analogRead(analogPin2);
    meas2 = map(meas2, 0, 1023, 0, 180);
    msg[0] = meas2;
    Serial.println(msg[0]);
    radio.write(msg, 1);
   
}
