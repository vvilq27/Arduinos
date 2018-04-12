#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 

#define SAMPLE_RATE 24000

int adcPin = A0;
int adcValue = 0;
//volatile int ledToggle = 0;
volatile byte buffIndex = 0;
volatile int packPerSec = 0;
volatile int lastPcktCnt = 0;
volatile byte buffEmpty = 0;
volatile byte volume = 4;
byte bytH;
int lastPckNum = 0;
int talkButton = 3;
int ledPin = 10;
RF24 myRadio(7,8);
byte addresses[][6] = {"253"};
byte buffer[32];

struct package{
  int id = 1;
  float temperature = 18.3;
//  char text[100] = "text to be transmitted";
};
typedef struct package Package;
Package data;

/*==========================
 *        METHODS
 ==========================*/
void populateBuffer(byte num){
  for(int i = 0; i <32; i++){
    buffer[i] = i + num;
  }
}

void timer1Audio(void){
  ICR1 = 10 * (1600000/SAMPLE_RATE);                  //Timer will count up to this value from 0;
  TCCR1A = _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1);  //Enable the timer port/pin as output
  TCCR1A |= _BV(WGM11);                              //WGM11,12,13 all set to 1 = fast PWM/w ICR TOP
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);      //CS10 = no prescaling
}


/*==========================
 *        SETUP
 ==========================*/
void setup()
{
  timer1Audio();
  
  Serial.begin(115200);
  delay(1000);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_LOW);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openWritingPipe( addresses[0]);
  delay(100);
  sei();

/*
  TCCR1A = 0;
  TCCR1B = 0;
  
  // preload timer 65536-16MHz/256/2Hz
  TCNT1 = 1000;   // preload timer
  TCCR1B |= (1 << CS12 | 1 << CS10);    // 256 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  interrupts();             // enable all interrupts
*/
  pinMode(ledPin,OUTPUT);
  pinMode(talkButton, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(talkButton), talk, CHANGE);
}

/*==========================
 *       MAIN LOOP
 ==========================*/
void loop()
{
  //displays last packet
  if(lastPckNum != data.id){
    Serial.println(adcValue);
    Serial.println(packPerSec);
  
    Serial.print("Sent package id: ");
    Serial.println(data.id);
    Serial.print("ADC: ");
    Serial.println(data.temperature);
  //sent number of packets sent in data packet and compare with packet cnt in receiver
    Serial.print("Packets sent per sec: " );
    Serial.println(packPerSec);
    lastPckNum =data.id;
  }
  

}

//interrupt set only when change on pin, when state is heldit wont trigger int?
void talk()
{
  if (!digitalRead(talkButton)) {
    while(!digitalRead(talkButton)){
      digitalWrite(ledPin, HIGH);
     /* adcValue = analogRead(adcPin);
      data.id = data.id + 1;
      data.temperature = adcValue;
  
      myRadio.write(&data, sizeof(data));
      */
//      buffer[0] = buffer[0] + 1;
//      populateBuffer(buffIndex++);
      myRadio.writeFast(&buffer, 32);
    }
  }else {
    digitalWrite(ledPin, LOW);
  }
}

ISR(TIMER1_OVF_vect)        // interrupt service routine
{

}

//voice data sending
ISR(TIMER1_COMPA_vect){                                         // This interrupt vector sends the samples when a buffer is filled
   if(buffEmpty == 0){                             // If a buffer is ready to be sent                               // Get the buffer # before allowing nested interrupts
      TIMSK1 &= ~(_BV(OCIE1A));                      // Disable this interrupt vector
      sei();                                         // Allow other interrupts to interrupt this vector (nested interrupts)
      myRadio.writeFast(&buffer,32);
      TIMSK1 |= _BV(OCIE1A);                           // Re-enable this interrupt vector
   }
}

//voice data acqusition
ISR(TIMER1_COMPB_vect){                                            // This interrupt vector captures the ADC values and stores them in a buffer
  if(buffEmpty == 1)
    buffer[buffIndex++] = ADCH;                    // Read the high byte of the ADC register into the buffer for 8-bit samples         
  else{
    
  }
//    OCR1A = bytH << volume;
}
