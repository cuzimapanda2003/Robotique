#include <Servo.h>

Servo myservo;
int pos = 0;
int pot = A1;
int potValue = 0;
int potValueMap;

void setup() {
myservo.attach(8);
}

void loop() {

  potValue = analogRead(pot);
  pos = map(potValue, 0, 1023, 0, 180);

    myservo.write(pos);

  }

