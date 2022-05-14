#include "stepper_motor_28byj48.h"

StepperMotor28byj48 motor(12,11,10,9);

void setup() {
  Serial.begin(9600);
  motor.begin();
}

void loop() {
  motor.cwTurn();
  delay(3000);
  motor.ccwTurn();
  delay(3000);
}



