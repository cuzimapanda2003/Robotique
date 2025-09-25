unsigned long currentTime;

void setup() {
pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

 currentTime = millis();

 blinkLED();
}

void blinkLED() {

  static unsigned long lastTime = 0;
 static bool ledState = LOW;


  const unsigned long rate = 200;

 if (currentTime - lastTime >= rate) {
      // Inversion de l'état du DEL
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState);
      // Sauvegarde du temps actuel
      lastTime = currentTime;
    }
}
