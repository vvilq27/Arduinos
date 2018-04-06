#include <avr/io.h>
#include <avr/interrupt.h>

#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

uint8_t i = 0;

void setup() {
  // initialize the output pin, say PC0
  Serial.begin(9600);
   pwm_init();
   adc_init(0);
   timer2init();
   sei();
   DDRC |= (1 << PC4);//test pin
  DDRD |= (1 << PD6);//test pin
//  OCR0A = 128;
  
}

void loop() {
//  while(ADCH > TCNT2)
//    PORTD |= (1<< PD6);
//  PORTD &= ~(1<< PD6); 

//while(ADCH > TCNT2)
//  PORTC |= (1<< 4);
//PORTC &= ~(1<< 4);
  

}

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);//Vref - Vcc, left adjust result
  ADCSRA = 0;
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS0 )|( 1<<ADPS1 ); //ADC on, 2 prescaler
  //pomiary na kanale A0
  PIN_PORT(ADC_PORT) &= ~(1<<channels);
}

inline uint8_t pomiar(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
  return ADCH;
}

void timer2init(void){
  TCCR2B |= (1 << CS22) | (1 << CS21); // presc 256; f = 62khz
  TIMSK2 |= (1<<TOIE2); // ovf int enable
}

void pwm_init()
{
    // initialize TCCR0 as per requirement, say as follows
    TCCR0A |= (1<<WGM01)|(1<<COM0A1);//(1<<WGM01)|
    TCCR0B |=  (1<<CS00);//(1<<WGM02)|
    // make sure to make OC0 pin (pin PB3 for atmega32) as output pin
    DDRD |= (1<<PD6);
}

ISR(TIMER2_OVF_vect){
  OCR0A = pomiar(0);
  Serial.println(OCR0A);
}
