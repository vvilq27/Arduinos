#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 

#define BUFFER_SIZE 28

RF24 myRadio(7,8);
byte addresses[][6] = {"253"};

struct Audio{
  byte id = 1;
  byte audioBuf[BUFFER_SIZE];
};
Audio audioPack;

byte volatile audioIndex;
byte volatile displayDone = 1;
int volatile rcvPacketCnt = 0;

/*
 *        FUNCTIONS
 */

void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11)| _BV(COM1B0) | _BV(COM1B1);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  TIMSK1 |= _BV(TOIE1);
  ICR1 = 888;
  DDRB |= _BV(PB1)| _BV(PB2);   // pwm pin as output  D9
  OCR1B = 455;
}

/*
void adcAudio(void){
  //adc continous mode
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1)| (1 << ADPS0); //set ADC clock with 128 prescaler- 16mHz/128 9khz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
   ADCSRA |= (1 << ADIE);
}
*/

void setup() {
  timer1FastPwmAInit();

  Serial.begin(115200);
  
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_1MBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  sei();
}//end setup

/*============================
 *        MAIN LOOP
 =============================*/
void loop() {
  
  if ( myRadio.available() ) { //&& displayDone
    while (myRadio.available()){
       myRadio.read(&audioPack, sizeof(audioPack));
    }

    rcvPacketCnt++;
//    displayDone = 0;
Serial.print(audioPack.id);
Serial.print(" ");
    Serial.println(audioPack.id-rcvPacketCnt);
/*
    for(int i =0; i <5; i++){
      for(int j =0; j <5; j++){
        Serial.print(audioPack.audioBuf[i*5 + j]);
        Serial.print(" ");
      }
      Serial.println();
    }
 */
  }
  
  
  /* works on .write(
if ( myRadio.available() ) { //&& displayDone
    while (myRadio.available()){
          myRadio.read(&fooData, sizeof(fooData));
    }
    Serial.println(fooData);
  }
*/

}//end main loop

ISR(TIMER1_OVF_vect){
  OCR1A = OCR1B = audioPack.audioBuf[audioIndex++];
  if(audioIndex > BUFFER_SIZE){
    audioIndex = 0;
    displayDone = 1;
  }
}
