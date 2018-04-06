//fast pwm at OC1A pin (D9) using timer1
// ICR1 is used as timer top value
// OCR1A is used as timer duty cycle value

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Servo.h>

#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

Servo ESC;
int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int value = 0;

//void timer2init(void){
//  TCCR2B |= (1 << CS22) | (1 << CS21); // presc 256; f = 62khz
//  TIMSK2 |= (1<<TOIE2); // ovf int enable
//}

void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);// | _BV(CS10);
  ICR1 = 4;
  DDRB |= _BV(PB1);   // pwm pin as output  D9
  OCR1A = 2;
}

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);//Vref - Vcc, left adjust result
  ADCSRA = 0;
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS0 )|( 1<<ADPS1 ); //ADC on, 2 prescaler
  //pomiary na kanale A0
  PIN_PORT(ADC_PORT) &= ~(1<<channels);
  
}

inline void pomiar(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
}

//================================
//             setup
//================================
void setup() {
  timer1FastPwmAInit();
  sei();
}


//================================
//           main loop
//================================

//timers interrupts disabled
void loop() {
  
}

ISR(TIMER2_OVF_vect){

}

ISR(TIMER1_OVF_vect){
  
}


