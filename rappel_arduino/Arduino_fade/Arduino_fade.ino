int led = 8;
int brightness = 0;
int fade = 5;
void setup() {
  // put your setup code here, to run once:
 pinMode(led, OUTPUT);
}

void loop() {
  analogWrite(led, brightness);

  brightness = brightness + fade;

   if (brightness <= 0 || brightness >= 255) {
    fade = -fade;
  }

  delay(30);
 

}
