#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

volatile uint16_t sum100;
volatile float avg;
volatile uint8_t tim_cnt;
volatile uint16_t sum_cnt;

void setup() {
  adc_init(0);
  tim2_init();
  Serial.begin(115200);

  pinMode(12, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

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

void tim2_init(void){
  TCCR2B |=  (1<<CS21) | (1<<CS22); // clkT2S/8 prescale
  TIMSK2 |= (1<<TOIE2); // ovf int enable
}

ISR(TIMER2_OVF_vect){
    meas(0);
    sum100 += ADCH;
    if(sum_cnt++ > 99){
      Serial.print(sum100);
      if(sum100 < 20600)
        digitalWrite(12, HIGH);
      else
        digitalWrite(12, LOW);
      avg = (float)sum100/255/sum_cnt;
      avg *= 4.5;
      sum100 = 0;
      Serial.print("\t");
      Serial.print(avg );
      Serial.print("\t");
      Serial.println(ADCH );
      
      sum_cnt = 0;
    }

}
