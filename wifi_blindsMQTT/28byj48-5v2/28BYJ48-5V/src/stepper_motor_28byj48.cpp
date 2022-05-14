#include "stepper_motor_28byj48.h"

StepperMotor28byj48::StepperMotor28byj48(int pinA, int pinB, int pinC, int pinD){
  _pinA = pinA;
  _pinB = pinB;
  _pinC = pinC;
  _pinD = pinD;
}

void StepperMotor28byj48::begin(){
  pinMode(_pinA, OUTPUT);
  pinMode(_pinB, OUTPUT);
  pinMode(_pinC, OUTPUT);
  pinMode(_pinD, OUTPUT);
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, LOW);
}

void StepperMotor28byj48::cwTurn(){ 
  for(int i = 1; i <= 512; i++){
    cwStep();
  }
}

void StepperMotor28byj48::ccwTurn(){ 
  for(int i = 1; i <= 512; i++){
    ccwStep();
  }
}

void StepperMotor28byj48::cwStep(){
  miniStep1();
  miniStep2();
  miniStep3();
  miniStep4();
  miniStep5();
  miniStep6();
  miniStep7();
  miniStep8();
  miniStep0();
}

void StepperMotor28byj48::ccwStep(){
  miniStep8();
  miniStep7();
  miniStep6();
  miniStep5();
  miniStep4();
  miniStep3();
  miniStep2();
  miniStep1();
  miniStep0();
}

void StepperMotor28byj48::miniStep0(){ // turn power off to all windings of the motor
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, LOW);
}
void StepperMotor28byj48::miniStep1(){
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, HIGH);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep2(){
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, HIGH);
  digitalWrite(_pinD, HIGH);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep3(){
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, HIGH);
  digitalWrite(_pinD, LOW);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep4(){
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, HIGH);
  digitalWrite(_pinC, HIGH);
  digitalWrite(_pinD, LOW);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep5(){
  digitalWrite(_pinA, LOW);
  digitalWrite(_pinB, HIGH);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, LOW);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep6(){
  digitalWrite(_pinA, HIGH);
  digitalWrite(_pinB, HIGH);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, LOW);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep7(){
  digitalWrite(_pinA, HIGH);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, LOW);
  delay(_motoDelay);
}

void StepperMotor28byj48::miniStep8(){
  digitalWrite(_pinA, HIGH);
  digitalWrite(_pinB, LOW);
  digitalWrite(_pinC, LOW);
  digitalWrite(_pinD, HIGH);
  delay(_motoDelay);
}

