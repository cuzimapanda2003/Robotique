//Marc-Antoie Blais 2168637

#include <MeAuriga.h>

const int m1_pwm = 11;
const int m1_in1 = 48; 
const int m1_in2 = 49; 

//Motor Right
const int m2_pwm = 10;
const int m2_in1 = 47; 
const int m2_in2 = 46;

enum State{normal, ralenti, danger};
State currentState = normal;

enum dangerState{stop, recule, turn};
dangerState state2 = stop;

#define LEDNUM  12
#define LEDPIN  44
#define RINGALLLEDS 0

MeRGBLed led( PORT0, LEDNUM );

MeUltrasonicSensor ultraSensor(PORT_10);

long currentTime = 0;
long previousTime = 0;
long delayTime = 500;

bool dangerCycleComplete = false;  // indique si on a fini le cycle danger

void setup() {
  Serial.begin(9600);

  pinMode(m1_pwm, OUTPUT);  
  pinMode(m1_in2, OUTPUT);  
  pinMode(m1_in1, OUTPUT);

  pinMode(m2_pwm, OUTPUT);  
  pinMode(m2_in2, OUTPUT);  
  pinMode(m2_in1, OUTPUT);

  led.setpin(LEDPIN);
}

void loop() {
  currentTime = millis();

  // Si on est en danger, on veut exécuter tout le cycle complet avant de checker la distance
  if(currentState == danger && !dangerCycleComplete) {
    stateDanger();
  } else {
    // Sinon on peut lire la distance et changer d’état normalement
    dist();
    stateMachine();
  }
}

void stateMachine(){
  switch (currentState) {
    case normal:
      if (state2 == stop) { 
        avance();
        ledTaskVert();
      }
      break;

    case ralenti:
      if (state2 == stop) { 
        slowdown();
        ledTaskBlue();
      }
      break;

    case danger:
      // en théorie ici on ne passe jamais car dangerCycleComplete bloque
      ledTaskRed();
      break;
  }
}

void avance(){
  int maxPwm = 153;
  digitalWrite(m1_in2, LOW);
  digitalWrite(m1_in1, HIGH);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, LOW);
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_pwm, maxPwm);
}

void slowdown(){
  int maxPwm = 76;
  digitalWrite(m1_in2, LOW);
  digitalWrite(m1_in1, HIGH);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, LOW);
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_pwm, maxPwm);
}

void ledTaskVert() {
  led.setColor(0, 0, 0, 0);
  for (int i = LEDNUM / 2 + 1; i <= LEDNUM; i++) {
    led.setColor(i, 0, 255, 0);
  }
  led.show(); 
}

void ledTaskBlue(){
  led.setColor(0, 0, 0, 0);
  for (int i = 1; i <= LEDNUM / 2; i++) {
    led.setColor(i, 0, 0, 255);
  }
  led.show(); 
}

void ledTaskRed(){
  led.setColor(0, 0, 0, 0);
  for (int i = 1; i <= LEDNUM; i++) {
    led.setColor(i, 255, 0, 0);
  }
  led.show(); 
}

void dist(){
  float distCm = ultraSensor.distanceCm();

  if(currentState == danger){
    // On sort du mode danger que si on a fini le cycle et que la distance est suffisante
    if(dangerCycleComplete){
      if(distCm >= 80){
        currentState = normal;
        state2 = stop;
        dangerCycleComplete = false; // reset pour futur danger
      }
      else if(distCm < 80 && distCm >= 40){
        currentState = ralenti;
        state2 = stop;
        dangerCycleComplete = false; // reset pour futur danger
      }
      // si toujours proche <40 on reste en danger et on devra relancer cycle dans loop
    }
    // si pas cycle fini on ne change rien
    return;
  }

  // Si pas en danger, mise à jour simple
  if(distCm >= 80){
     currentState = normal;
     state2 = stop;  
  }
  else if(distCm < 80 && distCm >= 40){
    currentState = ralenti;
    state2 = stop; 
  }
  else if(distCm < 40){
    currentState = danger;
    state2 = stop;
    previousTime = currentTime;
    dangerCycleComplete = false;  // on démarre un nouveau cycle danger
  }
}

void stateDanger(){
  ledTaskRed();

  switch (state2) {
    case stop:
      stopRobot();
      break;

    case recule:
      reculeRobot();
      break;

    case turn:
      turnLeftRobot();
      break;
  }
}

// ARRÊT pendant delayTime ms
void stopRobot(){
  int maxPwm = 0;
  digitalWrite(m1_in2, LOW);
  digitalWrite(m1_in1, HIGH);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, LOW);
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_pwm, maxPwm);

  if (currentTime - previousTime >= delayTime) {
      state2 = recule;
      previousTime = currentTime;
  }
}

// RECUL pendant delayTime ms
void reculeRobot(){
  int maxPwm = 76;
  digitalWrite(m1_in2, HIGH);
  digitalWrite(m1_in1, LOW);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, HIGH);
  digitalWrite(m2_in1, LOW);
  analogWrite(m2_pwm, maxPwm);

  if (currentTime - previousTime >= delayTime) {
      state2 = turn;
      previousTime = currentTime;
  }
}


void turnLeftRobot(){
  int maxPwm = 150;
  int delayTime2 = 1000;
  digitalWrite(m1_in2, HIGH);
  digitalWrite(m1_in1, LOW);
  analogWrite(m1_pwm, maxPwm);

  digitalWrite(m2_in2, LOW);
  digitalWrite(m2_in1, HIGH);
  analogWrite(m2_pwm, maxPwm);

  if (currentTime - previousTime >= delayTime2) {
      state2 = stop;
      previousTime = currentTime;
      dangerCycleComplete = true; 
  }
}
