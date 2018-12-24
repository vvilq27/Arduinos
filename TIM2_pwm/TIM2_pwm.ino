

void setup() {
 //Clear OC2A on Compare Match, set OC2A at BOTTOM,
    TCCR2A |= (1<<COM2A1);
    //pwm mode 3
    TCCR2A |= (1<<WGM21) | (1<<WGM20);
    
    TCCR2B |= (1<<CS20) | (1<<CS21) | (1<<CS22); // clkT2S/8 prescale
    // OC2RA pin made as output 
    DDRB |= (1<<PB3);
    //compared with TCNT2 reg
    OCR2A = 50;
}

void loop() {
  // put your main code here, to run repeatedly:

}
