#include <avr/pgmspace.h>
#include <EEPROM.h>

int val;
int tempPin = 2;
uint16_t w = 0;
float getFloat;
float read;
int volatile godzina;
int volatile minuta;
bool set = false;

/*
float arr[100];
arr[10] = 5;
arr[20] = 69;
*/

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  /*for(a=0; a<1024; a+=2){
    read = EEPROM.read(a);
    Serial.print(a);
    Serial.print("\t");
    Serial.print(read);
    Serial.print("\t");
    read = EEPROM.read(++a);
    Serial.print(read);
    a--;
    Serial.println();
  }*/
  
  /*
 for(int a =0; a<100; a++){
  Serial.println(arr[a]);
 }*/

 for(int a = 0; a<4*256; a+=4){
  read = EEPROM.get(a, getFloat);
    Serial.println(read);
 }

}
void loop()
{
  val = analogRead(tempPin);
  float cel =  val*0.48828125; 
  
  if(!set){
    EEPROM.put(w, cel);
    w+= sizeof(float);
  }
  
  if(w>1024){
      set = true;
  }

  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(299200);

  /*czas(0,46);
  Serial.print("TEMPRATURE = ");
  Serial.print(cel);
  Serial.print("*C");
  Serial.print("\t");
  Serial.print(w);
  Serial.println();*/
}


// mozna to tez zrobic na return godzina i czas
int czas(int startg,int startm){
  if(!set){
    godzina = startg;
    minuta = startm;
    set = true;
  }
  
  minuta+=5;
  if(minuta>60){
    godzina++;
    minuta = 0;

    if(godzina>23){
      godzina = 0;
    }
  }

  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(299200);

  Serial.print(godzina);
  Serial.print(":");
  Serial.print(minuta);
  Serial.print("\t");
}


