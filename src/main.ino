#include <Arduino.h>

const int light = 13;
int buttonLastState = 0;
bool ligthState = false;

void setup() {
  Serial.begin(9600);
  pinMode(light, OUTPUT);
  pinMode(12, INPUT_PULLUP);
}

void loop() {
  boolean buttonState = !digitalRead(12);
  // on click function
  if(buttonState==1 && buttonLastState==0) {
    buttonLastState=1;
  } if(buttonState==0 && buttonLastState==1) {
    buttonLastState=0;
    ligthState = !ligthState;
  }

  digitalWrite(light, ligthState ? HIGH : LOW);
  
  // print status button
  Serial.println("Button: "); Serial.print(buttonState);

  delay(10); 
}
