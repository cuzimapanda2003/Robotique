#include <Servo.h>

Servo myservo;
int pos = 0;

void setup() {
  // put your setup code here, to run once:
myservo.attach(8);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (pos = 0; pos <= 180; pos += 1) { 
    myservo.write(pos); // Indiquer la position
    delay(15);          // Attendre 15ms pour laisser le temps
  }
  // Aller de 180 à 0
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }
}
