#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 

RF24 myRadio(7,8);
byte addresses[][6] = {"253"};

byte volatile fooData = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_1MBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

}

void loop() {
  // put your main code here, to run repeatedly:

  if ( myRadio.available() ) { //&& displayDone
    while (myRadio.available()){
          myRadio.read(&fooData, sizeof(fooData));
    }
    Serial.println(fooData);
  }
}
