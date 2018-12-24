/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN

const uint64_t pipe = 0xE8E8F0F0E1LL;

typedef struct{
   uint8_t spd;
   uint8_t srv1;
   uint8_t srv2;
   unsigned long timestamp;
} Bird_msg;

Bird_msg msg;

void setup() {
  Serial.begin(115200);
  
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1,pipe);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  radio.startListening();
  
  radio.printDetails();
  Serial.println("Receiver start");
}
void loop() {
  while(radio.available()) {
    radio.writeAckPayload( 1, &msg, sizeof(msg) );
    radio.read(&msg, sizeof(msg));
    Serial.print(msg.timestamp);
    Serial.print(" ");
    Serial.print(msg.spd);
    Serial.print(" ");
    Serial.print(msg.srv1);
    Serial.print(" ");
    Serial.println(msg.srv2);
  }
}
