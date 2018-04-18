#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 

#define BUFFER_SIZE 30
#define LED 3

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
volatile byte vol = 1;

/*==========================
 *        FUNCTIONS
============================*/

void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11)| _BV(COM1B0) | _BV(COM1B1);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  TIMSK1 |= _BV(TOIE1);
  ICR1 = 420;
  DDRB |= _BV(PB1)| _BV(PB2);   // pwm pin as output  D9
  OCR1B = 123;
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

void T2_init(void){
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); //1024 presc
  TIMSK2 |= (1 << TOIE2);
}

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR) | channels;//Vref - Vcc, left adjust result
  ADCSRA = 0;
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS2 )|( 1<<ADPS0 ); //ADC on, 32 prescaler
}

inline void pomiar(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
}

/*============================
 *          SETUP
 =============================*/
void setup() {
  timer1FastPwmAInit();

  adc_init(3);
  T2_init();  //int 
  
  Serial.begin(115200);
  Serial.println(ICR1);
  
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_1MBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  sei();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  delay(900);
  digitalWrite(LED, LOW);
}//end setup

/*============================
 *        MAIN LOOP
 =============================*/

 //add other buffer, maybe it helpsMM
void loop() {
  if ( myRadio.available() ){
    while (myRadio.available()){
       myRadio.read(&audioPack, sizeof(audioPack));
    }

/*
    for(int i =0; i <5; i++){
      for(int j =0; j <5; j++){
        Serial.print(audioPack.audioBuf[i*5 + j]);
        Serial.print(" ");
      }
      Serial.println();
    }
 */
//    TCCR1A |= _BV(COM1A1)| _BV(COM1B0) | _BV(COM1B1);//turn on speaker
  }//end if

}//end main loop

/*=========================
 *       INTERUPTS
=========================== */


ISR(TIMER2_OVF_vect){
//   pomiar(0);
//   ICR1 = ADCH * 256;
//   pomiar(1);
//   vol = ADCH / 25;
//   Serial.print(ICR1);
//   Serial.print("  ");
   Serial.println(OCR1A);
   
}

//przezucic wpis do rejestrow do innego ISR?
//pomierzyc czasy wykonania przerwan timerem
//ustawic timer do obslugi jakis funkcjonalnosci(reset diody)
//stworzyc buffer na 1kb ram i tam ladowac przychodzace dane
ISR(TIMER1_OVF_vect){
    OCR1A = OCR1B = audioPack.audioBuf[audioIndex++] * vol + 150;

  if(audioIndex > BUFFER_SIZE){
    audioIndex = 0;
//    displayDone = 1;
//    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1)); //turn off speaker
  }
}
