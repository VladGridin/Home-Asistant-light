#include <Arduino.h>

const int light = 13;

void setup() {
  Serial.begin(9600);
  pinMode(light, OUTPUT);
  pinMode(12, INPUT_PULLUP);
}

void loop() {
  static boolean ButtonState = !digitalRead(12);
  
  digitalWrite(light, ButtonState ? HIGH : LOW);
  
  // print status button
  Serial.println("Button: "); Serial.print(ButtonState);

  delay(10); 
}
