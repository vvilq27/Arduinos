#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 

#define SAMPLE_RATE 24000

RF24 myRadio(7,8);
volatile int ledToggle = 0;
volatile int recPcktCnt = 0;
volatile int lastPcktCnt = 0;
volatile int pcktsPerSec = 0;
volatile byte audioPcktCnt = 0;


void timer1Audio(void){
  ICR1 = 10 * (1600000/SAMPLE_RATE);                  //Timer will count up to this value from 0;
  TCCR1A = _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1);  //Enable the timer port/pin as output
  TCCR1A |= _BV(WGM11);                              //WGM11,12,13 all set to 1 = fast PWM/w ICR TOP
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);      //CS10 = no prescaling
}

void tim1Setup(void){
  TCCR1A = 0;
  TCCR1B = 0;

  // preload timer 65536-16MHz/256/2Hz
  TCNT1 = 1000;   // preload timer
  TCCR1B |= (1 << CS12 | 1 << CS10);    // 256 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  interrupts();             // enable all interrupts

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
}

struct package{
  int id = 0;
  float temperature = 0.0;
  char text[100] = "empty";
};

byte addresses[][6] = {"253"};
byte buffer[32];

typedef struct package Package;
Package data;


void setup()
{
  Serial.begin(115200);
  delay(1000);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
  Serial.print("Starting... \n");
//  tim1Setup();
  timer1Audio();
  sei();

  while( !myRadio.available());
  myRadio.read( &data, sizeof(data) );
  recPcktCnt = data.id;
  
}

void loop()  
{
  if ( myRadio.available()) 
  { 
    digitalWrite(10, HIGH);
    if(myRadio.isAckPayloadAvailable());
    while (myRadio.available())
    {
//      myRadio.read( &data, sizeof(data) );
       myRadio.read(&buffer, sizeof(buffer));

       for(int i = 0; i <5; i++){
        Serial.print(buffer[i*5]);
        Serial.print("  ");
        Serial.print(buffer[i*5+1]);
        Serial.print("  ");
        Serial.print(buffer[i*5+2]);
        Serial.print("  ");
        Serial.print(buffer[i*5+3]);
        Serial.print("  ");
        Serial.println(buffer[i*5+4]);
        
       }
/*
      recPcktCnt++;
    
      Serial.print("Package received ID:");
      Serial.println(data.id);
      Serial.print("received pckts count: ");
      Serial.println(recPcktCnt);
      Serial.print("ADC: ");
      Serial.println(data.temperature);
      Serial.print("received pckts/sec: ");
      Serial.println(pcktsPerSec);
      Serial.print("packet loss: ");
      Serial.println(data.id - recPcktCnt);
      */
    }
  }
  digitalWrite(10, LOW);
}

ISR(TIMER1_OVF_vect)        // interrupt service routine
{

//  if(ledToggle){
//    digitalWrite(10, HIGH);
//    ledToggle = 0;
//  } else{
//  digitalWrite(10, LOW);
//    ledToggle = 1;
//  }
  
  
//  TCNT1 = 34286 + 34286/2;
//  
//  pcktsPerSec = data.id - lastPcktCnt;
//  lastPcktCnt = data.id;

//  WK TK part

  OCR1A = OCR1B = (buffer[audioPcktCnt] << 8);
  audioPcktCnt++;

  if(audioPcktCnt > 32)
    audioPcktCnt = 0;
}



