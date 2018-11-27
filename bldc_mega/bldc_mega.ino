//AD2 - IN1
//AD3 - IN3
//AD4 - IN1v2
//AD5 - ENA
//AD6 - ENB
//AD7 - ENAv2

#define DEBUG 1                   //macro used, to disable hall code part;
                                  //1 - hall disabled, 0 - hall enabled
#define ADC_PORT F
#define PIN_PORT(x) sPIN_PORT(x)  //just more verbose macro, 
#define sPIN_PORT(x) (DDR##x)     //to set port's pins as either input or output

volatile uint8_t phase;
volatile bool motor_stop;
volatile bool motor_direction;

//========================
//         SETUP
//========================
void setup() {
  DDRA |= B11111100;                          //set driver pins as output

  //Hall pin setup
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  pinMode(A10,INPUT);
  
  //PK3:5 pullup resistor
  pinMode(A11,INPUT_PULLUP);
  pinMode(A12,INPUT_PULLUP);
  pinMode(A13,INPUT_PULLUP);
  
  //TIM2 setup
  TCCR2A |= (1<<WGM21) | (1<<WGM20);          //tim2 in fast pwm mode
  TCCR2B |= (1<<CS20) | (1<<CS22)| (1<<CS21); //tim prescaler 1024
  TIMSK2 |= (1<<TOIE2) | (1<<OCIE2A);         //tim2 overflow and output compare A interrupt enable

  sei();        //global interrupt enable

  adc_init(0);  //setup adc for pwm
  phase = 1;    //set motor initial phase

  //EXTI setup, pin change mode
  PCICR |= (1 << PCIE2);      //PCINT16:23 pins enabled
  PCMSK2 |= B00111111;        //PCINT16:21 pins unmasked

  //motor setup
  motor_stop = false;
  motor_direction = true;

#if DEBUG == 1
  Serial.begin(250000);
#endif
}

//========================
//       MAIN LOOP
//========================
void loop() {
}

//========================
//   CUSTOM FUNCTIONS
//========================
//adc setup function
void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);               //Vref - Vcc, left adjust result
  ADCSRA = 0;                                       //resest register, for safety reasons
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS0 )|( 1<<ADPS1 );  //ADC on, 2 prescaler
  PIN_PORT(ADC_PORT) &= ~(1<<channels);             //set adc channel
}

//adc measuring function
inline void meas(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete
}

void turn_one_way(){
  switch(phase){
      case 1:
        PORTA = B10100100;
        break;
      case 2:
        PORTA = B01100100;
        break;
      case 3:
        PORTA = B11010000;
        break;
      case 4:
        PORTA = B10110000;
        break;
      case 5:
        PORTA = B01101000;
        break;
      case 6:
        PORTA = B11001000;
        break;
    }//end switch  
}

void turn_the_other_way(){
    switch(phase){
      case 6:
        PORTA = B10100100;
        break;
      case 5:
        PORTA = B01100100;
        break;
      case 4:
        PORTA = B11010000;
        break;
      case 3:
        PORTA = B10110000;      
        break;
      case 2:
        PORTA = B01101000;
        break;
      case 1:
        PORTA = B11001000;
        break;
    }//end switch
}

void turn_motor(){
  if(motor_direction)
    turn_one_way();
  else
    turn_the_other_way();
}

//========================
//          ISR
//========================
ISR(TIMER2_OVF_vect){
    meas(0);        //get pwm
    OCR2A = ADCH;   //write pwm value

    //motor action
    if(motor_stop)
      PORTA = B00000000;
      
#if DEBUG == 1
    Serial.print(motor_direction);
    Serial.print("  ");
    Serial.print(motor_stop);
    phase++;        //move to next phase
    //repeat sequence
    if(phase == 7)  //cant be in switch case
       phase = 1;
#endif
}

ISR(TIMER2_COMPA_vect){
  //disable all outputs to simulate pwm
  PORTA &= B00000011;
}

//EXTI port(all pins on port K) layout(first two MSB not used):
// B 7  6     5        4        3        2        1        0
//  [ ][ ][PCINT21][PCINT20][PCINT19][PCINT18][PCINT17][PCINT16]
//
//PK0:2 HALL sensor inputs
//PK3 turn motor one way
//PK4 turn motor the other way
//PK5 turn off motor(driver port set low)
//
ISR(PCINT2_vect){
  //phase change based on HALL readings
#if DEBUG == 0
  if(  (PINK & B00000101) && phase == 6  ){   
    phase = 1;
  }

  if(  (PINK & B00000100) && phase == 1 ){   
    phase = 2;
  }

  if(  (PINK & B00000110) && phase == 2 ){   
    phase = 3;
  }

  if(  (PINK & B00000010) && phase == 3 ){   
    phase = 4;
  }

  if(  (PINK & B00000011) && phase == 4 ){   
    phase = 5;
  }

  if(  (PINK & B00000001) && phase == 5 ){   
    phase = 6;
  }
  turn_motor(); 
#endif

//  MOTOR BUTTON CONTROL
  if(~PINK & B00001000){
    motor_direction = true;
  }
  if(~PINK & B00010000){
    motor_direction = false;
  }
}
