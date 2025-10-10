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

// stabilisation sans delay
bool waitingForGyro = false;
unsigned long gyroWaitStart = 0;
const unsigned long gyroWaitDuration = 200; // ms

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
  gyro.update();
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
  const double kp = 1.5;
  const double kd = 0.8;

  gyro.update(); // toujours mettre à jour avant lecture

  if (firstRun) {
    zAngleGoal = gyro.getAngleZ();
    previousError = 0;
    return;
  }

  double currentAngle = gyro.getAngleZ();
  double error = currentAngle - zAngleGoal;
  double output = kp * error + kd * (error - previousError);
  previousError = error;

  short leftPWM = constrain(-speed - output, -255, 255);
  short rightPWM = constrain(speed - output, -255, 255);

  encoderLeft.setTarPWM(leftPWM);
  encoderRight.setTarPWM(rightPWM);
}

void avancerCm(int distanceCm, short speed = 100) {
  long startL = encoderLeft.getCurPos();
  long startR = encoderRight.getCurPos();
  long targetTicks = (long)(distanceCm * 39.267);

  long lastTime = millis();
  bool done = false;

  while (!done) {
    long now = millis();
    if (now - lastTime >= 10) {
      lastTime = now;
      encoderLeft.loop();
      encoderRight.loop();

      keepStraight(speed); // la référence doit avoir été initialisée avant d'entrer ici

      long posL = abs(encoderLeft.getCurPos() - startL);
      long posR = abs(encoderRight.getCurPos() - startR);
      long avgPos = (posL + posR) / 2;

      if (avgPos >= targetTicks) done = true;
    }
  }

  encoderLeft.setTarPWM(0);
  encoderRight.setTarPWM(0);
}

void turnRight90(short speed = 35) {
  static bool turning = false;

  if (!turning) {
    gyro.resetData();
    turning = true;
    waitingForGyro = false;
  }

  gyro.update();
  double currentAngle = fabs(gyro.getAngleZ());

  if (currentAngle < 85 && !waitingForGyro) {
    encoderLeft.setTarPWM(speed);
    encoderRight.setTarPWM(speed);
  } else if (!waitingForGyro) {
    encoderLeft.setTarPWM(0);
    encoderRight.setTarPWM(0);
    gyroWaitStart = millis();
    waitingForGyro = true;
  } else if (millis() - gyroWaitStart >= gyroWaitDuration) {
    waitingForGyro = false;
    turning = false;

    gyro.resetData();
    keepStraight(0, true); // définit le nouveau cap avant la prochaine avance

    stateCount++;
    if (stateCount == 1 || stateCount == 3) state = avanceGrand;
    else if (stateCount == 2) state = avancePetit;
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
      keepStraight(0, true); // initialise le cap avant d'avancer
      avancerCm(distCour, 100);
      state = tourne;
      break;

    case avanceGrand:
      ledTaskVert();
      keepStraight(0, true); // initialise le cap avant d'avancer
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
