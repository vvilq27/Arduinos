#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

void setup() {
  TCCR2A |= (1<<WGM21) | (1<<WGM20) | (1<<COM2A1);          //tim2 in fast pwm mode
  TCCR2B |= (1<<CS22) ; //tim prescaler 1024 | (1<<CS20)
  TIMSK2 |= (1<<TOIE2) | (1<<OCIE2A);

  pinMode(11, OUTPUT);
  sei();

  adc_init(0);
//  adc_init(1);
  Serial.begin(1000000);
}

void loop() {

}

ISR(TIMER2_OVF_vect){
    meas(0);
    meas(0);
    OCR2A = ADCH;
    Serial.println(ADCH);
//    meas(1);
//    meas(1);
//    Serial.println(analogRead(A1));
}

ISR(TIMER2_COMPA_vect){
  
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

