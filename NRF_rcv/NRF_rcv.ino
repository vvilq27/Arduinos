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
#include <Servo.h>

//#define debug

RF24 radio(7, 8); // CE, CSN
Servo motor;
Servo elevator;
Servo rudder;

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
  radio.setPALevel(RF24_PA_MAX);
  radio.enableAckPayload();
  radio.startListening();
  
  radio.printDetails();
  Serial.println("Receiver start");

  motor.attach(A2);
  elevator.attach(A1);
  rudder.attach(A0);
  
  sei();
//  wdt_enable(WDTO_4S);  //set watchdog to count for 4 sec until restart
//  WDTCSR = (1<<WDIE);
}

long pause;

void loop() {
  if(radio.available()) {
    Serial.println(pause);
    radio.writeAckPayload( 1, &msg, sizeof(msg) );
    radio.read(&msg, sizeof(msg));
    
#ifdef debug
    Serial.print(msg.timestamp);
    Serial.print(" ");
    Serial.print(msg.spd);
    Serial.print(" ");
    Serial.print(msg.srv1);
    Serial.print(" ");
    Serial.println(msg.srv2);
#endif

//    OCR1A = msg.spd; test
    motor.write(map(msg.spd, 0, 255, 60, 180));
    elevator.write(map(msg.srv1, 0, 255, 0, 180));
    rudder.write(map(msg.srv2, 0, 255, 0, 180));
    
    pause = millis();

//    wdt_reset(); // tell watchdog everything is fine, reset WDT timer
  }
  //&& !radio.available()

  if((millis() - pause) > 300){
    //doesnt work
    motor.write(map(3, 0, 255, 0, 180));
    elevator.write(map(80, 0, 255, 0, 180));
    Serial.println("lost");
    delay(50);
    while(true);
//    resetFunc();  
  }
}

void timer2init(void){
  TCCR2A |= (1<<WGM21)  |   (1<<WGM20);
  TCCR2B |= (1<<WGM22)   |  (1 << CS21)   |   (1 << CS20); // presc 1024; f = 16khz |   (1 << CS22)
  TIMSK2 |= (1<<TOIE2)   ; // ovf int enable //int f=60hz |   (1<<OCIE2A)
}

//malfunctioning
void timer1FastPwmAInit(void){
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12) | _BV(CS10);
  ICR1 = 2560;  //sets period
  DDRB |= _BV(PB1);   // pwm pin as output  D9
  OCR1A = 1;    //sets duty

  TIMSK1 |= (1<<TOIE1);
}

//ISR(TIMER2_OVF_vect){
//}

//not entering timer dunno why
//ISR(TIMER1_OVF_vect){
//  Serial.println(millis());
//  if((millis() - pause) > 400){
//    Serial.println("lost");
//    motor.write(0);
//    elevator.write(45);
//    resetFunc();  
//  }
//}

//enable it, read datasheet
ISR(WDT_vect){
  Serial.println("BYE!"); 
}

