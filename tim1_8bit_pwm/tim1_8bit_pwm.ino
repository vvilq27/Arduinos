#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

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

void setup() {
  //8bit fast pwm mode
  TCCR1A = (1<<WGM10) | (1<<COM1A1) | (1<<COM1B1);
  TCCR1B = (1<<WGM12);
  TCCR1B |= (1<<CS12) | (1<<CS10);  //1024 presc
  TIMSK1 |= (1<<TOIE1);
  adc_init(0);
  sei();

  DDRB |= B00000110;
}

void loop() {
  // put your main code here, to run repeatedly:

}

ISR(TIMER1_OVF_vect){
  meas(0);
  OCR1AL = ADCH;
  OCR1BL = ADCH;
  
}

