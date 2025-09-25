// Marc-Antoine Blais
// 2168637

#include <Servo.h>

Servo myservo;
const int led = 10;
const int pot = A7;

int potValue = 0;
int pourcentage = 0;
int pos = 0;
int valeur = 100;

enum Phase { LUMIERE, BRAIN };
Phase modeActuel = BRAIN;

void setup() {
  Serial.begin(9600);
  myservo.attach(2);
  pinMode(led, OUTPUT);


  for (int i = 0; i < 4; i++) {
    analogWrite(led, 255);
    delay(125);
    analogWrite(led, 0);
    delay(125);
  }

  Serial.println("Envoyez 's' pour le mode BRAIN, 'd' pour le mode LUMIERE, ou un nombre entre 0 et 100 pour régler la luminosité.");
}

void brainPhase() {
  analogWrite(led, 0);
  potValue = analogRead(pot);
  pos = map(potValue, 0, 1023, 0, 180);
  myservo.write(pos);
}

void lightPhase() {
  static unsigned long lastTime = 0;
  static bool ledState = LOW;

  potValue = analogRead(pot);
  unsigned long currentTime = millis();
  unsigned long rate = map(potValue, 0, 1023, 0, 2000);
  int brightness = map(valeur, 0, 100, 0, 255);

  if (currentTime - lastTime >= rate) {
    ledState = !ledState;
    analogWrite(led, ledState ? brightness : 0);
    lastTime = currentTime;
  }
}

void lireCommande() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "s") {
      modeActuel = BRAIN;
      Serial.println("Mode : BRAIN");
    } else if (input == "d") {
      modeActuel = LUMIERE;
      Serial.println("Mode : LUMIERE");
    } else {
     valeur = input.toInt();
      if (valeur >= 0 && valeur <= 100) {
        int brightness = map(valeur, 0, 100, 0, 255);
        analogWrite(led, brightness);
        Serial.print("Luminosité réglée à : ");
        Serial.print(valeur);
        Serial.println("%");
      } else {
        Serial.println("Valeur invalide. Entrez un nombre entre 0 et 100.");
      }
    }
  }
}

void loop() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= 1000) {
    potValue = analogRead(pot);
    pourcentage = map(potValue, 0, 1023, 0, 100);
    Serial.print("2168637 - ");
    Serial.print(pourcentage);
    Serial.println("%");
    lastTime = currentTime;
  }

  lireCommande();

  switch (modeActuel) {
    case BRAIN:
      brainPhase();
      break;
    case LUMIERE:
      lightPhase();
      break;
  }
}