#include <avr/io.h>

#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

#define out1 2
#define out2 3
#define out3 4

#define EN1 5
#define EN2 6
#define EN3 7

#define hall1 8
#define hall2 9
#define hall3 10

uint8_t phase;
volatile uint8_t duty;
volatile uint8_t cnt;
int Delay=4000;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;

void setup() {
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);
  pinMode(hall1, OUTPUT);
  pinMode(hall2, OUTPUT);
  pinMode(hall3, OUTPUT);
  pinMode(12, OUTPUT);

  //Clear OC2A on Compare Match, set OC2A at BOTTOM,
//  TCCR2A |= (1<<COM2A1);
  //pwm mode 3
  TCCR2A |= (1<<WGM21) | (1<<WGM20);
  
  TCCR2B |= (1<<CS20); // (1<<CS22)| (1<<CS21); // clkT2S/8 prescale
  TIMSK2 |= (1<<TOIE2) | (1<<OCIE2A); // ovf int enable
  
  // OC2RA pin made as output 
  DDRB |= (1<<PB3);
  OCR2A = 50;

  sei();
  
  adc_init(0);
  phase = 1;

  currentMillis = micros();
}

//========================
//       MAIN LOOP
//========================

void loop() {
}

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);//Vref - Vcc, left adjust result
  ADCSRA = 0;
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS0 )|( 1<<ADPS1 ); //ADC on, 2 prescaler
  //pomiary na kanale A0
  PIN_PORT(ADC_PORT) &= ~(1<<channels);
}

inline void meas(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
}

void turn_left(){
      switch(phase++){
      case 6://    |  |
        PORTD = B10100100;
        phase = 1;
        break;
      case 5:
        PORTD = B01100100;
        break;
      case 4:
        PORTD = B11010000;
        break;
      case 3:
        PORTD = B10110000;      
        break;
      case 2:
        PORTD = B01101000;
        break;
      case 1:
        PORTD = B11001000;
        break;
    }//end switch
}

void turn_right(){
  switch(phase++){
      case 1://    |  |
        PORTD = B10100100;
        break;
      case 2:
        PORTD = B01100100;
        break;
      case 3:
        PORTD = B11010000;
        break;
      case 4:
        PORTD = B10110000;      
        break;
      case 5:
        PORTD = B01101000;
        break;
      case 6:
        PORTD = B11001000;
        phase = 1;
        break;
    }//end switch  
}

ISR(TIMER2_OVF_vect){
  turn_right(); 
}

ISR(TIMER2_COMPA_vect){
  PORTD = 0;
  meas(0);
  OCR2A = ADCH;
}
