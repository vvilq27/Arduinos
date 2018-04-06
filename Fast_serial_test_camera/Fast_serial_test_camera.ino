void timer2init(void){
  TCCR2B |=  (1 << CS20); // no prescalling
  TIMSK2 |= (1<<TOIE2); // ovf int enable
}
volatile long val = 0;

void setup() {
  
  Serial.begin(1000000);
  timer2init();
  sei();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}

ISR(TIMER2_OVF_vect){
  Serial.println(val++);
  if(val>3000)
    digitalWrite(LED_BUILTIN, HIGH);
}
