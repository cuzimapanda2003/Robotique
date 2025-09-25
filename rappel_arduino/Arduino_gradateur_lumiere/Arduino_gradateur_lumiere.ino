int pot = A1;
int led = 8;
int potValue = 0;
int potValueMap;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led, OUTPUT);

}

void loop() {
  potValue = analogRead(pot);
  potValueMap = map(potValue, 0, 1023, 0, 255);
  analogWrite(led, potValueMap);

}
