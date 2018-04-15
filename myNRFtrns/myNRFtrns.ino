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
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1)| (1 << ADPS0); //set ADC clock with 128 prescaler- 16mHz/128 9khz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  ADCSRA |= (1 << ADIE);
}

/*=========================
 *        SETUP
=========================== */
void setup() {
  pinMode(3, INPUT_PULLUP);
  
  Serial.begin(115200);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_1MBPS ) ; 
  myRadio.openWritingPipe( addresses[0]);
  
  adcAudio();

attachInterrupt(digitalPinToInterrupt(3), reset, FALLING);
}//end of setup

//enabling next send
void reset(){
  resetPrint = 1;
}


/*=========================
 *        LOOP
=========================== */
void loop() {
  // put your main code here, to run repeatedly:
}

/*=========================
 *       INTERUPTS
=========================== */
//getting data and sending it
ISR(ADC_vect){
  //fill buffer before sending it
  if(audioIndex<BUFFER_SIZE)
    audioPack.audioBuf[audioIndex++] = ADCH;
  //if buffer full
  else{
    ADCSRA &= ~(1 << ADIE);
    
    if(resetPrint){
      if(myRadio.write(&audioPack, sizeof(audioPack)))
        Serial.println("packet sent");
      else{
        Serial.println("send error");
        ADCSRA |= (1 << ADIE);
        resetPrint = 0;
        audioIndex = 0;
        return;
      }
      
      for(int i =0; i <5; i++){
        for(int j =0; j <5; j++){
          Serial.print(audioPack.audioBuf[i*5 + j]);
          Serial.print(" ");
        }
        Serial.println();
      }
      
      Serial.println(audioPack.id++);
      resetPrint = 0;
      audioIndex = 0;
    }
    ADCSRA |= (1 << ADIE);
  }
}

