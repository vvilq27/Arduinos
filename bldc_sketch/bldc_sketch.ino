//sketch with hall and buttons, no pwm
#define DEBUG 1 

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


  //TIM2 setup
  TCCR2B |= (1<<CS20) | (1<<CS22)| (1<<CS21); 
  TIMSK2 |= (1<<TOIE2); 

  sei();

  phase = 1;
  motor_stop = false;

  Serial.begin(115200);
  
  PCICR |= (1 << PCIE0);
  PCMSK0 |= B00111111;//(1 < PCINT5) | (1 < PCINT3) | (1 < PCINT4);  //Set pin D10 trigger an interrupt on state change.
//  EICRA = B00000011;  //rising edge
}

void loop() {
  if(~PINB & B00000100){
    if(motor_stop == true){
      motor_stop = false;
    }else{
      motor_stop = true;
    }
    while(~PINB & B00000100);
    delay(300);
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

  if(~PINB & B00000100){
  }

//not working
//  if(~PINB & B00000100){
//    if(motor_stop == true){
//      motor_stop = false;
//    }else{
//      motor_stop = true;
//    }
//  }
}

//===================
//    FUNCTIONS
//===================

void turn_one_way(){
  switch(phase){
      case 1:
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
        break;
    }//end switch  
}

void turn_the_other_way(){
      switch(phase){
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

void turn_motor(){
  if(motor_direction)
    turn_one_way();
  else
    turn_the_other_way();
}

ISR(TIMER2_OVF_vect){
  if(motor_stop){
    PORTD = B00000000;
  }else{
    turn_motor();
    phase++;
    if(phase == 7)  //cant be in switch case
       phase = 1;
  }
  Serial.print("dir: ");
  Serial.print(motor_direction);
  Serial.print("\tmotor stop: ");
  Serial.println(motor_stop);
}
