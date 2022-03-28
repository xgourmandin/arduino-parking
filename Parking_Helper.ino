//www.elegoo.com
//2016.12.08
#include "SR04.h"
#define BTN_PIN 7
#define LED_R_PIN 2
#define LED_G_PIN 3
#define LED_B_PIN 4
#define TRIG_PIN 12
#define ECHO_PIN 11
#define IDLE_DELAY 3000
#define ACTIVE_DELAY 500
#define BLINK_DELAY 100

SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long currentDist;
long targetDistance;
long idleDistance;
long distEpsilon = 2;
long currentDelay = IDLE_DELAY;
boolean targetHitted = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  delay(1000);
}

void loop() {
  if (idleDistance == 0) {
    idleDistance = sr04.Distance();
    Serial.print("setting new idle distance to ");
    Serial.println(idleDistance);
  }
  if (digitalRead(BTN_PIN) == LOW) {
    targetDistance = sr04.Distance();
    Serial.print("setting new target distance to ");
    Serial.println(targetDistance);
  }
  currentDist = sr04.Distance();
  Serial.println(currentDist);
  long idleDistDiff = abs(currentDist - idleDistance);
  long targetDistDiff = abs(currentDist - targetDistance);
  if (idleDistDiff <= distEpsilon) {
    currentDelay = IDLE_DELAY;
    setLed(0, 0);
    targetHitted = false;
  } else if (targetDistDiff <= distEpsilon) {
    if (!targetHitted) {
      targetBlink();
      targetHitted = true;
    }
  } else {
    currentDelay = ACTIVE_DELAY;
    int red = max(min(map(targetDistDiff, 0, idleDistance, 10, 200), 200), 0);
    int green = max(min(map(targetDistDiff, 0, idleDistance - 100, 190, 0), 200), 0);
    Serial.print("red : ");
    Serial.println(red);
    Serial.print("green : ");
    Serial.println(green);
    setLed(red, green);
    targetHitted = false;
  }

  delay(currentDelay);
}

void setLed(int r, int g) {
  analogWrite(LED_R_PIN, r);
  analogWrite(LED_G_PIN, g);
}

void targetBlink() {
  for (int i = 0; i < 10; i++) {
    setLed(0, 200);
    delay(BLINK_DELAY);
    setLed(0, 0);
    delay(BLINK_DELAY);
  }
  setLed(0, 200);
}

unsigned long long superMillis() {
  static unsigned long nbRollover = 0;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis < previousMillis) {
    nbRollover++;
  }
  previousMillis = currentMillis;

  unsigned long long finalMillis = nbRollover;
  finalMillis <<= 32;
  finalMillis +=  currentMillis;
  return finalMillis;
}
