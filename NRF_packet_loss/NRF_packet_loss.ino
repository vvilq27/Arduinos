/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
#define SSD1306_LCDWIDTH  128
#define SSD1306_LCDHEIGHT 64

#define ADC_PORT C
#define PIN_PORT(x) sPIN_PORT(x)
#define sPIN_PORT(x) (DDR##x)

Adafruit_SSD1306 display(OLED_RESET);
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
const uint64_t pipeOut = 0xE8E8F0F0E1LL;

typedef struct{
   uint8_t spd;
   uint8_t srv1;
   uint8_t srv2;
   unsigned long timestamp;
} Bird_msg;

Bird_msg msg;
Bird_msg msg_back;


void setup() {
  radio_setup();
  oled_setup();
  Serial.begin(115200);

  adc_init(0);
  adc_init(1);
  adc_init(2);
}

unsigned int packetsSent = 0;
unsigned int packetsAcked = 0;
unsigned int latency = 0;

//===========================
//        LOOP
//===========================
void loop() {
  msg.timestamp = millis();
  
  meas(0);
  meas(0);
  msg.spd = meas(0);
  meas(1);
  meas(1);
  msg.srv1 = meas(1);
  meas(2);
  meas(2);
  msg.srv2 = meas(2);
  
  radio.write(&msg, sizeof(msg));
  packetsSent++;

  while ( radio.isAckPayloadAvailable()) {
    radio.read(&msg_back, sizeof(msg_back));
    latency = millis() - msg_back.timestamp;
    packetsAcked++;
  }

  radio.write(&msg, sizeof(msg));

  while ( radio.isAckPayloadAvailable()) {
    radio.read(&msg_back, sizeof(msg_back));
  }

  display.setCursor(0,0);
  display.print(latency);
  display.setCursor(50,0);
  display.println(msg_back.spd);
  display.print(packetsSent-packetsAcked);
  display.setCursor(50,10);
  display.println(msg_back.srv1);
  display.setCursor(50,20);
  display.println(msg_back.srv2);
  display.display(); 

//  Serial.print(now);
//  Serial.print("  ");
//  Serial.print(latency);
//  Serial.print("  ");
//  Serial.print(packetsSent);
//  Serial.print("  ");
//  Serial.println(packetsAcked);

  
  display.clearDisplay();
}

void radio_setup(){
  radio.begin();
  radio.openWritingPipe(pipeOut);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.stopListening();

//  memset(&timestamp, 0, sizeof(timestamp));
}

void oled_setup(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.println("hello world");
  display.display();
}

void adc_init(int channels){
  ADMUX = 0;
  ADMUX |= ( 1<<REFS0 ) | (1<<ADLAR);//Vref - Vcc, left adjust result
  ADCSRA = 0;
  ADCSRA |= ( 1<<ADEN )|( 1<<ADPS0 )|( 1<<ADPS1 ); //ADC on, 2 prescaler
  //pomiary na kanale A0
  PIN_PORT(ADC_PORT) &= ~(1<<channels);
}

inline uint8_t meas(uint8_t channel){
  ADMUX = (ADMUX & 0xF8)|channel;
  ADCSRA |= (1<<ADSC);// measure start
  while( ADCSRA & (1<<ADSC) ); // wait for meas to complete

  return ADCH;
}

