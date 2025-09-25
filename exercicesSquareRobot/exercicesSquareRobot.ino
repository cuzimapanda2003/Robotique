int maxPwm = 25;

const int m1_pwm = 11;
const int m1_in1 = 48; // M1 ENA
const int m1_in2 = 49; // M1 ENB

//Motor Right
const int m2_pwm = 10;
const int m2_in1 = 47; // M2 ENA
const int m2_in2 = 46; // M2 ENB

enum State{avance, tourne};
State currentState = avance;


void setup() {
  Serial.begin(9600);

  pinMode(m1_pwm, OUTPUT);  //We have to set PWM pin as output
  pinMode(m1_in2, OUTPUT);  //Logic pins are also set as output
  pinMode(m1_in1, OUTPUT);

  pinMode(m2_pwm, OUTPUT);  //We have to set PWM pin as output
  pinMode(m2_in2, OUTPUT);  //Logic pins are also set as output
  pinMode(m2_in1, OUTPUT);

}

void loop() {
//avanceWallie();
//turnLeftWallie();
//turnRightWallie();
squareWallie();

}

void avanceWallie() {
  digitalWrite(m1_in2, LOW);
  digitalWrite(m1_in1, HIGH);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, LOW);
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_pwm, maxPwm);
}

void turnRightWallie(){
  digitalWrite(m1_in2, HIGH);
  digitalWrite(m1_in1, LOW);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, LOW);
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_pwm, maxPwm);
}


void turnLeftWallie(){
  digitalWrite(m1_in2, LOW);
  digitalWrite(m1_in1, HIGH);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, HIGH);
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_pwm, maxPwm);
}


void squareWallie() {
  int currentTime = millis();
  static long statePrevious = 0;
  static long avanceDelay = 2000;
   static long turnDelay = 2000;


  switch (currentState) {
    case avance:
      avanceWallie();
      if (currentTime - statePrevious >= stateDelay) {
        currentState = tourne;
        statePrevious = currentTime;
      }
      break;

    case tourne:
      turnLeftWallie();
      if (currentTime - statePrevious >= stateDelay) {
        currentState = avance;
        statePrevious = currentTime;
      }
      break;
  }
}