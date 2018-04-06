
int analogPin = 3;
float data = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(2400);
}

void loop() {
  // put your main code here, to run repeatedly:

  data = (float) analogRead(analogPin)/200.0;
  data /= 1.09;
  Serial.println(data);
  delay(100);
}
