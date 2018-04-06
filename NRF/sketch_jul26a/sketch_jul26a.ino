#include <Servo.h>

Servo myservo;
int analogPin = 0; 
int analogPin2 = 1;
int meas = 0;
int meas2 = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(3);
}

void loop() {

    meas = analogRead(analogPin);
    meas2 = analogRead(analogPin2);
    meas2 = map(meas2, 0, 1023, 0, 180);
    myservo.write(meas2);
    //Serial.println(meas); // os Y
    Serial.println(meas2); // os X
}
