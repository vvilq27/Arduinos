//play radio to itself

#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 
#include <avr/interrupt.h>

#define SAMPLE_RATE 8000
#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

int adcPin = A0;
int adcValue = 0;
volatile byte buffIndex = 0;
volatile byte buffEmpty = 0;
volatile byte volume = 4;
volatile byte audioPcktCnt = 0;
byte bytH;



int talkButton = 2;
int ledPin = 10;
byte buffer[128];

/*==========================
 *        METHODS
 ==========================*/
void timer1Audio(void){
  ICR1 = 888;                  //Timer will count up to this value from 0;
  TCCR1A = _BV(COM1A1);// | _BV(COM1B0) | _BV(COM1B1);  //Enable the timer port/pin as output
  TCCR1A |= _BV(WGM11);                              //WGM11,12,13 all set to 1 = fast PWM/w ICR TOP
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);      //CS10 = no prescaling
  TIMSK1 |= _BV(TOIE1) | _BV(ICIE1) | _BV(OCIE1A);// | _BV(OCIE1B);
//  OCR1A = OCR1B = 500;
OCR1A = 700;
}

void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11)| _BV(COM1B0) | _BV(COM1B1);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  TIMSK1 |= _BV(TOIE1);
  ICR1 = 888;
  DDRB |= _BV(PB1)| _BV(PB2);   // pwm pin as output  D9
  OCR1B = 455;
}


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

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);//Vref - Vcc, left adjust result
  ADCSRA = 0;
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS2 )|( 1<<ADPS0 ); //ADC on, 32 prescaler

  PIN_PORT(ADC_PORT) &= ~(1<<channels);
}

inline void pomiar(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
}

/*==========================
 *        SETUP
 ==========================*/
void setup()
{
  
  //to display recorded audio
  timer1FastPwmAInit();
  //to record audio
  adcAudio();
  Serial.begin(1000000);

//  ADCSRB |= _BV(ADTS0) | _BV(ADTS0) | _BV(ADTS2);           //Attach ADC start to TIMER1 Capture interrupt flag
//  ADCSRA |= _BV(ADEN) | _BV(ADATE);
//  interrupts();             // enable all interrupts

  pinMode(talkButton, INPUT);
  pinMode(6, OUTPUT);
  pinMode(3, OUTPUT);
  
  //attachInterrupt(digitalPinToInterrupt(talkButton), talk, LOW);

  sei();
}

/*==========================
 *       MAIN LOOP
 ==========================*/
void loop()
{
  
}

//interrupt set only when change on pin, when state is heldit wont trigger int?
void talk(){
  digitalWrite(3, HIGH);
  while(!digitalRead(talkButton)) {
    //record audio
    Serial.println(OCR1A);
  }
  digitalWrite(3, LOW);
}

//not used/needed
ISR(TIMER1_CAPT_vect){
//  digitalWrite(6, HIGH);
  
}
//loads data to timer to play audio using pwms
ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  OCR1A = OCR1B = ADCH;
  if(!digitalRead(talkButton)) {
    //record audio
    Serial.println(OCR1A);
  }
}

//voice data display
//not used
ISR(TIMER1_COMPA_vect){                                         // This interrupt vector sends the samples when a buffer is filled
  /*
  OCR1A = OCR1B = (buffer[audioPcktCnt++] << 4);

  if(audioPcktCnt > 128){
    audioPcktCnt = 0;
    buffEmpty = 0;
  }
  */
//  digitalWrite(6, HIGH);
}

//voice data acqusition
//not used
ISR(TIMER1_COMPB_vect){                                            // This interrupt vector captures the ADC values and stores them in a buffer
    //buffer[audioPcktCnt++] = ADCH;                    // Read the high byte of the ADC register into the buffer for 8-bit samples         
//  digitalWrite(3, HIGH);
}

//not used/needed
ISR(ADC_vect){
//  Serial.println(ADCH);
}

