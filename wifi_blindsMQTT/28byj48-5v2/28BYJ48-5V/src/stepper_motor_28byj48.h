#ifndef stepper_motor_28byj48_h
#define stepper_motor_28byj48_h

#include <Arduino.h>

class StepperMotor28byj48{
  public:
    // constructor
    StepperMotor28byj48(int pinA, int pinB, int pinC, int pinD);
    void begin();
    void cwStep();
    void ccwStep();
    void cwTurn();
    void ccwTurn();
  private:
    int _pinA;
    int _pinB;
    int _pinC;
    int _pinD;
    int _motoDelay = 1;
    void miniStep0();
    void miniStep1();
    void miniStep2();
    void miniStep3();
    void miniStep4();
    void miniStep5();
    void miniStep6();
    void miniStep7();
    void miniStep8();
};



#endif
