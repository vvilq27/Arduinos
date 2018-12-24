#define ADC_PORT F
#define PIN_PORT(x) sPIN_PORT(x)  //just more verbose macro, 
#define sPIN_PORT(x) (DDR##x)     //to set port's pins as either input or output

#define SS_CNT 100

volatile uint8_t phase;
volatile bool motor_stop;
volatile bool motor_direction;
volatile uint16_t soft_start_cnt;
volatile bool soft_start;

//========================
//         SETUP
//========================

//Notes
//speed up timer doesnt start motor
void setup() {
  DDRA |= B11111100;                          //set driver pins as output

  //PK0:2 Hall pin setup
  pinMode(A8,INPUT);
  pinMode(A9,INPUT);
  pinMode(A10,INPUT);
  
  //PK3:5 pullup resistor
  pinMode(A11,INPUT_PULLUP);  //direction
  pinMode(A12,INPUT_PULLUP);  //direction
  
  pinMode(2,INPUT_PULLUP);//MOTOR START
  pinMode(3,INPUT_PULLUP);//MOTOR STOP
  
  TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20); //tim prescaler 1024
  TIMSK2 |= (1<<TOIE2) ;//| (1<<OCIE2A);         //tim2 overflow and output compare A interrupt enable

  sei();        //global interrupt enable

  adc_init(0);  //setup adc for pwm

//          VARIABLES
  get_phase();
  motor_stop = true;
  motor_direction = true;
  soft_start = true;
  soft_start_cnt = SS_CNT;
  unsigned long time;

  PCICR |= (1 << PCIE2);      //PCINT16:23 pins enabled    
  PCMSK2 |= (1<<PCINT19) | (1<<PCINT20);  //PCINT16:20 pins unmasked

//pin 3 stop, pin 2 start
  EICRB |= (1<<ISC41) | (1<<ISC51); //exti control reg; falling edge
  EIMSK |= (1<<INT4) | (1<<INT5); //exti mask

  Serial.begin(250000);
  Serial.println("1207");
  Serial.print("start phase: ");
  Serial.println(phase);
}

//========================
//       MAIN LOOP
//========================
void loop() {
  if(soft_start){
    Serial.println("SS");
    while(soft_start);
  }
  Serial.println(micros());
  
  uint8_t hall = PINK & B111;

  if(hall == B100){
    phase = 1;    
    turn_motor();
    Serial.println("H1");
    while((PINK & B111) == B100);
  }
  else if(hall == B101){
    phase = 2;    
    Serial.println("H2");
    turn_motor();
    while((PINK & B111) == B101);
  }
  else if(hall == B001){
    phase = 3;    
    turn_motor();
    Serial.println("H3");
    while((PINK & B111) == B001);
  }
  else if(hall == B011){
    phase = 4;    
    turn_motor();
    Serial.println("H4");
    while((PINK & B111) == B011);
  }
  else if(hall == B010){
    phase = 5;    
    turn_motor();
    Serial.println("H5");
    while((PINK & B111) == B010);
  }
  else if(hall == B110){
    phase = 6;    
    turn_motor();
    Serial.println("H6");
    while((PINK & B111) == B110);
  }
}

//========================
//          ISR
//========================
ISR(TIMER2_OVF_vect){
  if(soft_start_cnt ){  //&& !motor_stop
    turn_motor();
    phase++;        //move to next phase
    
    if(phase == 7)  //cant be in switch case
       phase = 1;
      
    if(motor_stop){
      PORTA &= B00000011;
    }
    soft_start_cnt--;
    Serial.println(soft_start_cnt);
    if(soft_start_cnt == 0)
      soft_start = false;
  }
  
  meas(0);        //get freq
  TCNT2 = ADCH;   //change timer freq
}

//  PCINT MOTOR BUTTON CONTROL
//PK3 turn motor one way
//PK4 turn motor the other way
ISR(PCINT2_vect){
  if(~PINK & (1<<PK3)){
    motor_direction = true;
    Serial.println("dir tru");
  }
  if(~PINK & (1<<PK4)){
    motor_direction = false;
    Serial.println("dir fls");
  }
}

//  EXTI MOTOR BUTTON CONTROL
//start
ISR(INT4_vect){
  Serial.println("int4 start");
  motor_stop = false;
  get_phase();
  soft_start = true;
  soft_start_cnt = SS_CNT;
}

//stop
ISR(INT5_vect){
  Serial.println("int5 stop");
  motor_stop = true;
}

//========================
//   CUSTOM FUNCTIONS
//========================
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
  if(!motor_stop){
    if(motor_direction)
      turn_one_way();
    else
      turn_the_other_way();
  }
}

void get_phase(){
  switch(PINK & B111){
    case B100:
      phase = 1;
      break;
    case B101:
      phase = 2;
      break;
     case B001:
      phase = 3;
      break;
     case B011:
      phase = 4;
      break;
     case B010:
      phase = 5;
      break;
     case B110:
      phase = 6;
      break;      
  }
  Serial.print("start phase: ");
  Serial.println(phase);
}

