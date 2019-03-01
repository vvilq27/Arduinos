void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);

}
long sum;
void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i < 30; i++){
    sum += analogRead(A0);
  }
  float avg_volt = (float) sum/30.0;
  sum = 0;
  
  float bat_float_voltage = (float) map(avg_volt, 0, 1023, 0, 1000)/215.0;
  Serial.print(bat_float_voltage);
  if(bat_float_voltage > 3.94){
    digitalWrite(9, HIGH);
    digitalWrite(13, LOW);
    Serial.print(" ");
    Serial.println(" ON ");
  }
   else{
    digitalWrite(9, LOW);
    digitalWrite(13, HIGH);
    Serial.print(" ");
    Serial.println(" OFF ");
   }
}
