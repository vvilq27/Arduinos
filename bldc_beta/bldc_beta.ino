//AD2 - IN1
//AD3 - IN3
//AD4 - IN1v2
//AD5 - ENA
//AD6 - ENB
//AD7 - ENAv2

#define DEBUG 0                   //macro used, to disable hall and Serial code part;
                                  //1 - hall disabled, 0 - hall enabled
#define SOFT_START_CNT 170
#define SOFT_START 0

#define ADC_PORT F
#define PIN_PORT(x) sPIN_PORT(x)  //just more verbose macro, 
#define sPIN_PORT(x) (DDR##x)     //to set port's pins as either input or output

volatile uint8_t phase;
volatile bool motor_stop;
volatile bool motor_direction;
volatile bool do_once;

#if SOFT_START == 1
volatile bool soft_start;
volatile uint16_t soft_start_cnt;
#endif

//========================
//         SETUP
//========================
void setup() {
  DDRA |= B11111100;                          //set driver pins as output

  //PK0:2 Hall pin setup
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  pinMode(A10,INPUT);
  
  //PK3:5 pullup resistor
  pinMode(A11,INPUT_PULLUP);
  pinMode(A12,INPUT_PULLUP);
  pinMode(A13,INPUT_PULLUP);
  
  //TIM2 setup
//  TCCR2A |= (1<<WGM21) | (1<<WGM20);          //tim2 in fast pwm mode
  TCCR2B |= (1<<CS22)| (1<<CS21); //tim prescaler 1024
  TIMSK2 |= (1<<TOIE2) ;//| (1<<OCIE2A);         //tim2 overflow and output compare A interrupt enable

  sei();        //global interrupt enable

  adc_init(0);  //setup adc for pwm

  //motor setup
  phase = 1;    //set motor initial phase
  motor_stop = true;
  motor_direction = true;
  do_once = false;

#if SOFT_START == 1
  soft_start = true;
  soft_start_cnt = SOFT_START_CNT;
#endif
  
  //EXTI setup, direction change only, pin change mode
  PCICR |= (1 << PCIE2);      //PCINT16:23 pins enabled
  PCMSK2 |= B00011000;        //PCINT16:20 pins unmasked

  Serial.begin(250000);
}

//========================
//       MAIN LOOP
//========================
void loop() {
  if(~PINK & B00100000){
    //resume motor
    if(motor_stop == true){
      motor_stop = false;

#if SOFT_START == 1
    soft_start = true;
    soft_start_cnt = SOFT_START_CNT;
#endif

    //stop motor  
    }else{
      motor_stop = true;
    }
    while(~PINK & B00100000);
    delay(300);
  }

    phase = 1;    
    do_once = false;
    turn_motor();
    Serial.println("H1");
  //hold while phase1
  while((PINK & B111) == B100);

    phase = 2;    
    Serial.println("H2");
    do_once = false;
    turn_motor();
  //hold while phase2
  while((PINK & B111) == B101);

    phase = 3;    
    do_once = false;
    turn_motor();
    Serial.println("H3");
  //hold while phase3
  while((PINK & B111) == B001);

    phase = 4;    
    do_once = false;
    turn_motor();
    Serial.println("H4");
  //hold while phase4
  while((PINK & B111) == B011);

    phase = 5;    
    do_once = false;
    turn_motor();
    Serial.println("H5");
  //hold while phase5
  while((PINK & B111) == B010);

    phase = 6;    
    do_once = false;
    turn_motor();
    Serial.println("H6");
  //hold while phase6
  while((PINK & B111) == B110);
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
        PORTA = B10100100;  // 101 001
        Serial.println("P1");
        break;
      case 2:
        PORTA = B01100100;  //011 001
        Serial.println("P2");
        break;
      case 3:
        PORTA = B11010000;  //110 100
        Serial.println("P3");
        break;
      case 4:
        PORTA = B10110000;  //101 100
        Serial.println("P4");
        break;
      case 5:
        PORTA = B01101000;  //011 010
        Serial.println("P5");
        break;
      case 6:
        PORTA = B11001000;  //110 010
        Serial.println("P6");
        break;
    }//end switch  
}

void turn_the_other_way(){
    switch(phase){
      case 6:
        PORTA = B10100100;
        Serial.println("PO6");
        break;
      case 5:
        PORTA = B01100100;
        Serial.println("P5");
        break;
      case 4:
        PORTA = B11010000;
        Serial.println("P4");
        break;
      case 3:
        PORTA = B10110000;
        Serial.println("P3");      
        break;
      case 2:
        PORTA = B01101000;
        Serial.println("P2");
        break;
      case 1:
        PORTA = B11001000;
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

//========================
//          ISR
//========================
ISR(TIMER2_OVF_vect){
  meas(0);        //get freq
  TCNT2 = ADCH;   //change timer freq

  if(motor_stop){
    PORTA &= B00000011;
  }else{
    if(!do_once){
//      turn_motor();
      do_once = true;
    }
  }
    
#if DEBUG == 1
  phase++;        //move to next phase
  //repeat sequence
  if(phase == 7)  //cant be in switch case
     phase = 1;
#endif

#if SOFT_START == 1
  if(soft_start){
    if(!soft_start_cnt--){
      soft_start = false;
    }
  }
#endif
}

//    ***** ISR DISABLED  *****

//ISR(TIMER2_COMPA_vect){
//  //disable all outputs to simulate pwm
//  PORTA &= B00000011;
//}

//EXTI port(all pins on port K) layout(first two MSB not used):
// B 7  6     5        4        3        2        1        0
//  [ ][ ][       ][PCINT20][PCINT19][       ][       ][       ]
//
//PK0:2 HALL sensor inputs
//PK3 turn motor one way
//PK4 turn motor the other way
//
ISR(PCINT2_vect){
  //phase change based on HALL readings

//  MOTOR BUTTON CONTROL
  if(~PINK & (1<<PK3)){
    motor_direction = true;
  }
  if(~PINK & (1<<PK4)){
    motor_direction = false;
  }
}
