#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

#define BUFFER_SIZE 30
#define LED_PIN 6

RF24 myRadio(7,8);
byte addresses[][6] = {"253"};

struct Audio{
  byte id = 1;
  byte audioBuf[BUFFER_SIZE];
};

Audio audioPack;

byte volatile fooData = 1;
byte volatile audioIndex;
byte volatile resetPrint = 1;

void adcAudio(void){
  //adc continous mode
  //128 presc - 9khz
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32 38khz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  ADCSRA |= (1 << ADIE);
}

void T2_init(void){
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); //1024 presc
  TIMSK2 |= (1 << TOIE2);
}


/*=========================
 *        SETUP
=========================== */
void setup() {
  pinMode(3, INPUT_PULLUP);

  T2_init();

  Serial.begin(115200);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_1MBPS ) ; 
  myRadio.openWritingPipe( addresses[0]);
  
  adcAudio();
  pinMode(LED_PIN, OUTPUT);
}//end of setup


/*=========================
 *        LOOP
=========================== */
void loop() {
  /*
  if(!digitalRead(3)){
  }
  */
}

/*=========================
 *       INTERUPTS
=========================== */

ISR(TIMER2_OVF_vect){
//   Serial.println(ADCH);
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

//getting data and sending it
ISR(ADC_vect){
  
  //fill buffer before sending it
  if(audioIndex<BUFFER_SIZE)
    audioPack.audioBuf[audioIndex++] = ADCH;
  //if buffer full
  else{
//    ADCSRA &= ~(1 << ADIE);
    myRadio.writeFast(&audioPack, sizeof(audioPack));
    audioIndex = 0;
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
}

//======== questions ===========
// how long does it take to send packet and it is possible to send it when new packet is formed
//what if i sent bigger packets than 32B

