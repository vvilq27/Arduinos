//sketch with hall and buttons, no pwm
#define DEBUG 1 

#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

//driver input pins
#define out1 2
#define out2 3
#define out3 4

//driver output enable pins
#define EN1 5
#define EN2 6
#define EN3 7

volatile uint8_t phase;
volatile bool motor_stop;
volatile bool motor_direction;
volatile bool soft_start;

//===================
//      SETUP
//===================
void setup() {
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(13, OUTPUT);


  //TIM2 setup
  TCCR2B |=  (1<<CS22)| (1<<CS21)| (1<<CS20); //(1<<CS20) 
  TIMSK2 |= (1<<TOIE2); 
  
  adc_init(0);

  phase = 1;
  motor_stop = true;
  motor_direction = true;
  soft_start = true;

  Serial.begin(250000);

//
  for (int i = 0; i<=600; i++) {
    turn_motor();
    phase++;
    if(phase == 7)  //cant be in switch case
       phase = 1;
    if(i < 70)
      delay(10);
    if(i > 69 && i < 170)
      delay(7);
    if(i > 169 && i < 220)
      delay(3);
    if(i > 219 )
      delay(2);
//    if(i > 219)
//      delay(1);
  }
//  soft_start = false;
  motor_stop = true;
  PORTD &= B00000011;
  
//  sei();
cli();
    
  PCICR |= (1 << PCIE0);
  PCMSK0 |= B00011111;//(1 < PCINT5) | (1 < PCINT3) | (1 < PCINT4);  //Set pin D10 trigger an interrupt on state change.
//  EICRA = B00000011;  //rising edge
}

void loop() {
  if(~PINB & (1<<PB2)){
    if(motor_stop == true){
      motor_stop = false;
    }else{
      motor_stop = true;
    }
    while(~PINB & (1<<PB2));
    delay(300);
  }

}

//===================
//    FUNCTIONS
//===================
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

void turn_one_way(){
  digitalWrite(13, LOW);
  switch(phase){
      case 1:
        PORTD = B10100100;
        Serial.println("P1");
        break;
      case 2:
        PORTD = B01100100;
        Serial.println("P2");
        break;
      case 3:
        PORTD = B11010000;
        Serial.println("P3");
        break;
      case 4:
        PORTD = B10110000;
        Serial.println("P4");
        break;
      case 5:
        PORTD = B01101000;
        Serial.println("P5");
        break;
      case 6:
        PORTD = B11001000;
        Serial.println("P6");
        break;
    }//end switch  
}
//prawy guzik D12
void turn_the_other_way(){
  digitalWrite(13, HIGH);
    switch(phase){
      case 6://    |  |
        PORTD = B10100100;
        Serial.println("PO6");
        break;
      case 5:
        PORTD = B01100100;
        Serial.println("P5");
        break;
      case 4:
        PORTD = B11010000;
        Serial.println("P4");
        break;
      case 3:
        PORTD = B10110000; 
        Serial.println("P3");     
        break;
      case 2:
        PORTD = B01101000;
        Serial.println("P2");
        break;
      case 1:
        PORTD = B11001000;
        Serial.println("P1");
        break;
    }//end switch
}

void turn_motor(){
  if(motor_direction)
    turn_one_way();
  else
    turn_the_other_way();
}

ISR(TIMER2_OVF_vect){
  meas(0);
  TCNT2 = ADCH;

  if(motor_stop)
      PORTD &= B00000011;
  else{
    turn_motor();
    phase++;
    if(phase == 7)  //cant be in switch case
       phase = 1;
  }
}

ISR(PCINT0_vect){
#if DEBUG == 0
  if(  (PINB & B00000101) && phase == 6  ){   
    phase = 1;    
  }

  if(  (PINB & B00000100) && phase == 1 ){   
    phase = 2;    
  }

  if(  (PINB & B00000110) && phase == 2 ){   
    phase = 3;    
  }

  if(  (PINB & B00000010) && phase == 3 ){   
    phase = 4;    
  }

  if(  (PINB & B00000011) && phase == 4 ){   
    phase = 5;    
  }

  if(  (PINB & B00000001) && phase == 5 ){   
    phase = 6;    
  }
#endif

  //    MOTOR CONTROL
  //d11
  if(~PINB & B00001000){
    motor_direction = true;
  }
  //d12
  if(~PINB & B00010000){
    motor_direction = false;
  }
}
