#include <MeAuriga.h>

int stateCount = 0;

int nbTuilAvanceCour = 3;
int nbTuilAvanceLong = 5;

int distLong = 30.48 * nbTuilAvanceLong;
int distCour = 30.48 * nbTuilAvanceCour;

enum State {
  avancePetit,
  avanceGrand,
  tourne,
  stop
};

State state = stop;


MeEncoderOnBoard encoderLeft(SLOT1);
MeEncoderOnBoard encoderRight(SLOT2);


MeGyro gyro(PORT0, 0x69);

#define LEDNUM  12
#define LEDPIN  44
#define RINGALLLEDS 0

MeRGBLed led( PORT0, LEDNUM );


void isr_process_encoder1(void) {
  if (digitalRead(encoderLeft.getPortB()) == 0) {
    encoderLeft.pulsePosMinus();
  } else {
    encoderLeft.pulsePosPlus();
  }
}


void isr_process_encoder2(void) {
  if (digitalRead(encoderRight.getPortB()) == 0) {
    encoderRight.pulsePosMinus();
  } else {
    encoderRight.pulsePosPlus();
  }
}

void setup() {
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
    gyro.resetData();
    zAngleGoal = gyro.getAngleZ();
    previousError = 0;
    Serial.println("Direction locked.");
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

  long targetTicks = (long)(distanceCm * 10.0 * 39.267);

  bool done = false;
  keepStraight(speed, true); 

  while (!done) {
    encoderLeft.loop();
    encoderRight.loop();

    keepStraight(speed); 

    long posL = abs(encoderLeft.getCurPos() - startL);
    long posR = abs(encoderRight.getCurPos() - startR);
    long avgPos = (posL + posR) / 2;

    if (avgPos >= targetTicks) {
      done = true;
    }
  }

  encoderLeft.setTarPWM(0);
  encoderRight.setTarPWM(0);
}



void loop() {
  encoderLeft.loop();
  encoderRight.loop();

  stateMachine();
}



void stateMachine(){

switch (state) {
    case avancePetit:
      ledTaskVert();
      avancerCm(distCour, 100);
      state = tourne;
      break;

    case avanceGrand:
    avancerCm(distLong, 100);
      state = tourne;
      break;

    case tourne:
    ledTaskBlue();


      if(stateCount == 1 || stateCount == 3){
        state = avancePetit;
        stateCount++;
      }
      if(stateCount == 0 || stateCount == 2){
        state = avanceGrand;
        stateCount++;
      }
      if(stateCount == 4){
        state = stop;
      }
      break;
      case stop:
      ledTaskRed();


      stateCount = 0;
      break;
  }


}


void ledTaskVert() {
  led.setColor(0, 0, 0, 0);
  for (int i = 1; i <= LEDNUM; i++) {
    led.setColor(i, 0, 255, 0);
  }
  led.show(); 
}

void ledTaskBlue(){
  led.setColor(0, 0, 0, 0);
  for (int i = 1; i <= LEDNUM; i++) {
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


void pauseStop() {

}



