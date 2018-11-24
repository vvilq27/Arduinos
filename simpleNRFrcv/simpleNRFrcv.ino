#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 
#include <printf.h> //print details

RF24 myRadio(7,8);
//byte addresses[][6] = {"253"};
const uint64_t pipe = 0x0a;

byte volatile fooData = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("RECEIVER");
  printf_begin(); //print details
  
  myRadio.begin();  
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.openReadingPipe(0, pipe);
  myRadio.startListening();
//  myRadio.setDataRate(RF24_250KBPS) ; 
//  myRadio.setChannel(115); 

  
  myRadio.printDetails();

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



