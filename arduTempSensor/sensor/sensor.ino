#include <avr/pgmspace.h>
#include <EEPROM.h>

int val;
int tempPin = 2;
int w = 0;
float read;

void setup()
{
Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

}
void loop()
{
  val = analogRead(tempPin);
  float cel =  (float)(val*0.48828125); 


  //EEPROM.write(w, cel);
  //w++;


  Serial.print("TEMPRATURE = ");
  Serial.print(cel);
  Serial.print("*C");
    Serial.print("\t");
      Serial.print(val);
  Serial.println();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(1000);

}
