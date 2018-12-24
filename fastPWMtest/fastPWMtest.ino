#include <avr/io.h>

void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  ICR1 = 888;
  DDRB |= _BV(PB1);   // pwm pin as output  D9
  OCR1A = 65;
}

void setup() {
  timer1FastPwmAInit();
  PORTB |= _BV(PB1);
  Serial.begin(1000000);
  pinMode(LED_BUILTIN, OUTPUT);
/*
  // put your setup code here, to run once:
  DDRB |= (1 << DDB1)|(1 << DDB2);
    // PB1 and PB2 is now an output

    ICR1 = 0xEFFF;
    // set TOP to 16bit

    OCR1AH = 0x00;
    // set PWM for 25% duty cycle @ 16bit

    OCR1B = 0x00FF;
    // set PWM for 75% duty cycle @ 16bit

    TCCR1A |= (1 << COM1A1) ;//|(1 << COM1B1);
    // set none-inverting mode

    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13);
    // set Fast PWM mode using ICR1 as TOP
    
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // START the timer with no prescaler

*/
/* works
 DDRD |= (1 << DDD6);
    // PD6 is now an output

    OCR0A = 6;
    // set PWM for 50% duty cycle


    TCCR0A |= (1 << COM0A1);
    // set none-inverting mode

    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    // set fast PWM Mode

    TCCR0B |= (1 << CS01);
    // set prescaler to 8 and starts PWM
*/

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  Serial.println(TCNT1);
  // put your main code here, to run repeatedly:

}
