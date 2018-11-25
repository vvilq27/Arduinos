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
  pinMode(10, OUTPUT);


  //TIM2 setup
  TCCR2B |= (1<<CS20) | (1<<CS22)| (1<<CS21); 
  TIMSK2 |= (1<<TOIE2); 

  sei();

  phase = 1;
  motor_stop = false;

  Serial.begin(115200);
//  attachInterrupt(0, turn_off, RISING);
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 < PCINT5) | (1 < PCINT3) | (1 < PCINT4);  //Set pin D10 trigger an interrupt on state change.
//  EICRA = B00000011;  //rising edge
}

void loop() {
}

ISR(PCINT0_vect){
  //d11
  if(PINB & B00001000){
    Serial.println("ONE"); 
    motor_direction = true;
  }
  //d12
  if(PINB & B00010000){
    Serial.println("OTHER");
    motor_direction = false;
  }
    
  //if D11 rised
//  if(PINB & B00100000)
//    motor_stop = true;
//  delay(500);
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
//  if(PINB & B00001000){
//    motor_stop = true;
//  }
//  else{
//    motor_stop = false;
//  }

  if(motor_stop){
    PORTD = B00000000;
  }else{
//    if(PINB & B00010000){
//      turn_the_other_way();
//    }else{
//      turn_one_way();
//    }
    turn_motor();
    phase++;
    if(phase == 7)  //cant be in switch case
       phase = 1;
  }
}
