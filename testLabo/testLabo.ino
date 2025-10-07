#include <MeAuriga.h>

int stateCount = 0;
int nbTuilAvanceCour = 1;
int nbTuilAvanceLong = 1;
int distCour = 30.48 * nbTuilAvanceCour;
int distLong = 30.48 * nbTuilAvanceLong;

enum State { avancePetit, avanceGrand, tourne, stop };
State state = stop;

MeEncoderOnBoard encoderLeft(SLOT1);
MeEncoderOnBoard encoderRight(SLOT2);
MeGyro gyro(0, 0x69);

#define LEDNUM 12
#define LEDPIN 44
MeRGBLed led(PORT0, LEDNUM);

long currentTime = 0;
long previousTime = 0;
long delayTime = 3000;

void isr_process_encoder1() {
  if (digitalRead(encoderLeft.getPortB()) == 0) encoderLeft.pulsePosMinus();
  else encoderLeft.pulsePosPlus();
}

void isr_process_encoder2() {
  if (digitalRead(encoderRight.getPortB()) == 0) encoderRight.pulsePosMinus();
  else encoderRight.pulsePosPlus();
}

void setup() {
  led.setpin(LEDPIN);
  attachInterrupt(encoderLeft.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(encoderRight.getIntNum(), isr_process_encoder2, RISING);
  Serial.begin(115200);
  gyro.begin();
  gyro.resetData();
  encoderLeft.setPulse(9);
  encoderRight.setPulse(9);
  encoderLeft.setRatio(39.267);
  encoderRight.setRatio(39.267);
  encoderLeft.setPosPid(1.8, 0, 1.2);
  encoderRight.setPosPid(1.8, 0, 1.2);
  encoderLeft.setSpeedPid(0.18, 0, 0);
  encoderRight.setSpeedPid(0.18, 0, 0);
}

void keepStraight(short speed = 100, bool firstRun = false) {
  static double zAngleGoal = 0.0;
  static double previousError = 0.0;
  const double kp = 3.0;
  const double kd = 1.0;
  if (firstRun) {
    zAngleGoal = gyro.getAngleZ();
    previousError = 0;
    return;
  }
  double error = gyro.getAngleZ() - zAngleGoal;
  double output = kp * error + kd * (error - previousError);
  previousError = error;
  encoderLeft.setTarPWM(speed - output);
  encoderRight.setTarPWM(-speed - output);
}

void avancerCm(int distanceCm, short speed = 100) {
  long startL = encoderLeft.getCurPos();
  long startR = encoderRight.getCurPos();
  long targetTicks = (long)(distanceCm * 39.267);
  bool done = false;
  keepStraight(-speed, true);
  while (!done) {
    encoderLeft.loop();
    encoderRight.loop();
    keepStraight(-speed);
    long posL = abs(encoderLeft.getCurPos() - startL);
    long posR = abs(encoderRight.getCurPos() - startR);
    long avgPos = (posL + posR) / 2;
    if (avgPos >= targetTicks) done = true;
  }
  encoderLeft.setTarPWM(0);
  encoderRight.setTarPWM(0);
}

void turnRight90(short speed = 100) {
  static bool turning = false;
  static long targetL = 0, targetR = 0;

  if (!turning) {
    // Démarre la rotation
    long startL = encoderLeft.getCurPos();
    long startR = encoderRight.getCurPos();
    encoderLeft.move(90, speed);
    encoderRight.move(-90, speed);
    targetL = startL + 90 * 39.267 / 360.0; // approximation selon ta config
    targetR = startR - 90 * 39.267 / 360.0;
    turning = true;
  }

  encoderLeft.loop();
  encoderRight.loop();

  // Vérifie si les moteurs ont atteint leur position cible
  bool doneL = abs(encoderLeft.getCurPos() - targetL) < 10;
  bool doneR = abs(encoderRight.getCurPos() - targetR) < 10;

  if (doneL && doneR) {
    turning = false;
    stateCount++;
    if (stateCount == 1 || stateCount == 3)
      state = avanceGrand;
    else if (stateCount == 2)
      state = avancePetit;
    else if (stateCount == 4) {
      state = stop;
      previousTime = currentTime;
    }
  }
}


void loop() {
  currentTime = millis();
  encoderLeft.loop();
  encoderRight.loop();
  stateMachine();
}

void stateMachine() {
  switch (state) {
    case stop:
      ledTaskRed();
      if (pauseStop()) {
        stateCount = 0;
        state = avancePetit;
      }
      break;
    case avancePetit:
      ledTaskVert();
      avancerCm(distCour, 100);
      state = tourne;
      break;
    case avanceGrand:
      ledTaskVert();
      avancerCm(distLong, 100);
      state = tourne;
      break;
    case tourne:
      ledTaskBlue();
      turnRight90();
      break;
  }
}

void ledTaskVert() {
  for (int i = 1; i <= LEDNUM; i++) led.setColor(i, 0, 255, 0);
  led.show();
}

void ledTaskBlue() {
  for (int i = 1; i <= LEDNUM; i++) led.setColor(i, 0, 0, 255);
  led.show();
}

void ledTaskRed() {
  for (int i = 1; i <= LEDNUM; i++) led.setColor(i, 255, 0, 0);
  led.show();
}

bool pauseStop() { return (millis() - previousTime >= delayTime); }
