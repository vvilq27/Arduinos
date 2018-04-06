// Receiver usb1

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

int msg[1];
bool done =false;
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
Servo myservo;

void setup(void){
   Serial.begin(9600);
   radio.begin();
   radio.openReadingPipe(1,pipe);
   radio.startListening();
   myservo.attach(3);

}
   
void loop(void){
   if (radio.available()){   
      done = radio.read(msg, 1);
      myservo.write(msg[0]);
      Serial.println(msg[0]);
   }
   else{
    Serial.println("No radio available");
   }
}
