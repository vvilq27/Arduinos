#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(10, 11); // RX, TX
SoftwareSerial mySerial(8, 9); // RX, TX

#define FRAME_TYPE_SIZE 6 //GPRMC + ',' = 6

//sudo tcpdump -i wlan0 UDP port 9999 -vv -X


uint8_t GPRMC_received_flag;
uint8_t frame_type_char_count;
char frame_type[FRAME_TYPE_SIZE];
uint8_t sentence_collected_flag;
uint8_t sentence_field_cnt;

char sentence[80];
volatile int sentence_index;
volatile uint8_t catching_time;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
//  gsmSerial.begin(9600);
  gsm_init();
  catching_time = 1;
  delay(500);
}

void loop() {
// debug input
//  Serial.write(mySerial.read());

  get_gps();
  if(catching_time == 0){
    Serial.println("now we sleep..");
    delay(15000);
    Serial.println("now we awake");
    catching_time = 1;
  }
//  gsmSerial.listen();
//
//  if (mySerial.available()) {
//    Serial.write(mySerial.read());
//  }

//  if (Serial.available()) {
//    Serial.write(Serial.read());
//  }

//  if(gsmSerial.available()){
//    Serial.write(gsmSerial.read());
//  }
//  if(Serial.available()){
//    gsmSerial.write(Serial.read());
//  }
}

void print(void) {
  for (int i = 0; i < sentence_index ; i++)
    Serial.write(sentence[i]);
  Serial.print(" ");
  Serial.print(sentence_index);
}

void gsm_init(void){
  gsmSerial.begin(9600);
  
  gsmSerial.write("AT+CGATT=1\r\n");
  delay(50);
  gsmSerial.write("AT+CSTT=\"Internet\",\"\",\"\"\r\n");
  delay(50);
  gsmSerial.write("AT+CIICR\r\n");
  delay(150);
  gsmSerial.write("AT+CIFSR\r\n");
  delay(50);
  gsmSerial.write("AT+CIPSTART=\"UDP\",\"89.78.88.104\",9999\r\n");
  delay(150);
}

int volatile sentenceStart;

void get_gps(void){
  mySerial.listen();
  while (mySerial.available()) {

    char data = mySerial.read();// get data from UART buffer\
    Serial.print(data);
    
    if (data == '$') {
      sentenceStart = 1;
      return;
    }

    if(data == '*'){
      if(GPRMC_received_flag){
        char sendCommand[16];
        Serial.println();
        print();    //print before index will be delated
        Serial.println();
        sprintf(sendCommand, "at+cipsend=%i\r\n", sentence_index);
        gsmSerial.write(sendCommand);
        delay(50);
        gsmSerial.write(sentence);
        catching_time = 0;
      }
      
      sentence_collected_flag = 1;
      frame_type_char_count = 0;
      sentence_field_cnt = 0;
      sentence_index = 0;
      GPRMC_received_flag = 0;

      for(int i = 0; i < FRAME_TYPE_SIZE; i++)
        frame_type[i] = 0;

      return;
    }

    if (frame_type_char_count < FRAME_TYPE_SIZE  && sentenceStart == 1) {
      frame_type[frame_type_char_count] = data;
      frame_type_char_count++;
      return;
    }

    if (GPRMC_received_flag == 0 && frame_type_char_count == FRAME_TYPE_SIZE) {
      frame_type_char_count =0;
      sentenceStart = 0;
      
      if (strncmp(frame_type, "GPRMC", 5))
        return;
      else{
        GPRMC_received_flag = 1;
      
      }
    }
  
    if(GPRMC_received_flag){
      if(data == 44)    //if comma
        sentence_field_cnt++;
        
      if(sentence_field_cnt < 9)
        sentence[sentence_index++] = data;
    }
  } //if serial available
} //get gps

//not used
void put_str(const char* s){
  while(*s)
    gsmSerial.write(*s++);
}








/*
    //collecting chars of GPRMC sentence
    // if received /r or $  or sentence collected then ignore it
    //     $ char unlocks               strncmp GPRMC unlocks          
    if( sentence_collected_flag == 0 && GPRMC_received_flag == 1 && data != 0xa && data != 0xd ){
      sentence[sentence_index++] = data;
      
//      Serial.print("=====got char====== ");
       Serial.print(" ");
      Serial.println(sentence_field_cnt);
      if(data == 44)    //if comma
        sentence_field_cnt++;

//        Serial.write(sentence[sentence_index-1]);
        Serial.write(data);

      //ignore data after DATE in gps frame
      if(sentence_field_cnt >= 9 ){
        Serial.println("=====print sentence======");
//        print();
        
        Serial.write('\n');
        
//        char sendCommand[16];
//        sprintf(sendCommand, "at+cipsend=%i\r\n",sentence_index);
//        Serial.write(sendCommand);
        
        sentence_collected_flag = 1;
        GPRMC_received_flag = 0;
        sentence_index = 0;
        
//        Serial.println("=====sentence completed=== ");
        
        return;
      }
    }
    */

















