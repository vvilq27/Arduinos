#define DEBUG 1

volatile int num;
volatile  bool test;
void setup() {
  // put your setup code here, to run once:
  DDRC = B00000100;
  TCCR2B |= (1<<CS20) | (1<<CS22)| (1<<CS21); //tim presc 1024
  TIMSK2 |= (1<<TOIE2);   //tim2 ovf interrupt enable

  pinMode(12, INPUT_PULLUP);
  pinMode(A2, OUTPUT);
  
  sei();
  Serial.begin(115200);
  pinMode(13, OUTPUT);  //led pin enables

}

void loop() {
  #if DEBUG != 0
  if((~PINB & B00010000)){
    digitalWrite(A2, HIGH);
    test = true;
  }
  else{
    PORTC &= ~(1<<PC2);
    test = false;
  }
#endif

}

ISR(TIMER2_OVF_vect){
  Serial.println(test);
  if(num++ > 30){
    Serial.println((PORTC & B00000100));
    num = 0;
//    PORTC ^= B00000100;
  }
  
}

