/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/wdt.h>

RF24 radio(7, 8); // CE, CSN

const uint64_t pipe = 0xE8E8F0F0E1LL;
int duty;

typedef struct{
   uint8_t spd;
   uint8_t srv1;
   uint8_t srv2;
   unsigned long timestamp;
} Bird_msg;

Bird_msg msg;

void (*resetFunc)(void) = 0;

void setup() {
  Serial.begin(115200);
  timer1FastPwmAInit();
  
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);
  
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1,pipe);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  radio.startListening();
  
  radio.printDetails();
  Serial.println("Receiver start");
  
  sei();
//  wdt_enable(WDTO_4S);  //set watchdog to count for 4 sec until restart
//  WDTCSR = (1<<WDIE);
}

int pause;

void loop() {
  while(radio.available()) {
    radio.writeAckPayload( 1, &msg, sizeof(msg) );
    radio.read(&msg, sizeof(msg));
    Serial.print(msg.timestamp);
    Serial.print(" ");
    Serial.print(msg.spd);
    Serial.print(" ");
    Serial.print(msg.srv1);
    Serial.print(" ");
    Serial.println(msg.srv2);

    OCR1A = msg.spd;
    pause = millis();

//    wdt_reset(); // tell watchdog everything is fine, reset WDT timer
  }
  //&& !radio.available()
}

void timer2init(void){
  TCCR2A |= (1<<WGM21)  |   (1<<WGM20);
  TCCR2B |= (1<<WGM22)   |  (1 << CS21)   |   (1 << CS20); // presc 1024; f = 16khz |   (1 << CS22)
  TIMSK2 |= (1<<TOIE2)   ; // ovf int enable //int f=60hz |   (1<<OCIE2A)
}

void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12) | _BV(CS10);
  ICR1 = 256;  //sets period
  DDRB |= _BV(PB1);   // pwm pin as output  D9
  OCR1A = 1;    //sets duty

  TIMSK1 |= (1<<TOIE1);
}

//ISR(TIMER2_OVF_vect){
//}

ISR(TIMER1_OVF_vect){
  if((millis() - pause) > 400){
    Serial.println("connection lost");
    resetFunc();  
  }
}

//enable it, read datasheet
ISR(WDT_vect){
  Serial.println("BYE!"); 
}

