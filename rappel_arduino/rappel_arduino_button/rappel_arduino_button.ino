short btnPin = 2; // Broche 2
short ledPin = LED_BUILTIN; // Broche 13

void setup() {
  // IMPORTANT! On configure la pin 2 en entrée 
  // avec une résistance de pullup
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // On lit l'état de la pin 2
  int buttonState = digitalRead(btnPin);

  // On écrit l'état dans la pin 13
  digitalWrite(ledPin, buttonState);
}