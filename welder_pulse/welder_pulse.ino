bool weld;
int duty;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(5, INPUT_PULLUP);

  weld = false;
  duty = 0;
}

void loop() {
  Serial.println(analogRead(A0));
  delay(1);
  
  if(!digitalRead(5)){
    duty = analogRead(A0);
    delay(3000);
    weld = true;
  }

  if(weld){
    weld = false;
    
    PORTD |= (1<<PD2);
    delay(40);
    PORTD &= ~(1<<PD2);
    
    delay(50);
    PORTD |= (1<<PD2);
    delay(duty);
    PORTD &= ~(1<<PD2);

    duty = 0;
  }
}
