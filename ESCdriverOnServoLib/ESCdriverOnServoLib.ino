#include <Servo.h> 
 
Servo myservo;  
 
int pos = 0;
 
void setup() { 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(50); 
  delay(1000);
  myservo.write(60); 
  delay(1000);
  myservo.write(70); 
  delay(1000);
  myservo.write(80); 
  delay(1000);
  myservo.write(90); 
  delay(1000);
  myservo.write(100); 
  delay(1000);
  for(pos = 0; pos < 179; pos += 1)  // goes from 0 degrees to 180 degrees 
    {                                  // in steps of 1 degree 
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
    } 
    
  for(pos = 179; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
    {                                
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
    } 
  delay(1000);
  myservo.write(50);
} 
 
void loop() {
  myservo.write(50); 
  delay(1000);
  myservo.write(60); 
  delay(1000);
  myservo.write(70); 
  delay(1000);
  myservo.write(80); 
  delay(1000);
  myservo.write(90); 
  delay(1000);
  myservo.write(100); 
  delay(1000);
  for(pos = 0; pos < 179; pos += 1)  // goes from 0 degrees to 180 degrees 
    {                                  // in steps of 1 degree 
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
    } 
  for(pos = 179; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
    {                                
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(10000);
  myservo.write(50);
}
