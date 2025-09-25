#include <HCSR04.h>
#include <DHT.h>
#include <

#define DHTPIN 4

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

HCSR04 hc(2, 3);

long currentTime = 0;
long dhtPrevious = 0;
long dhtDelay = 5000;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  currentTime = millis();


  if (currentTime - dhtPrevious >= dhtDelay) {
    dhtPrevious = currentTime;
    Serial.print("Distance : ");
    Serial.print(hc.dist());
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.print(" Humidité: ");
    Serial.print(h);
    Serial.print("%  Température: ");
    Serial.print(t);
    Serial.print("°C ");

  }
}
