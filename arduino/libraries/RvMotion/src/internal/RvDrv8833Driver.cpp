#include "internal/RvDrv8833Driver.h"

RvDrv8833Driver::RvDrv8833Driver(const RvDrv8833Pins &pins) : pins_(pins) {}

void RvDrv8833Driver::begin() {
  pinMode(pins_.leftIn1, OUTPUT);
  pinMode(pins_.leftIn2, OUTPUT);
  pinMode(pins_.rightIn1, OUTPUT);
  pinMode(pins_.rightIn2, OUTPUT);
  stop();
}

void RvDrv8833Driver::drive(int16_t leftPwm, int16_t rightPwm) {
  driveSingleMotor(pins_.leftIn1, pins_.leftIn2, clampPwm(leftPwm));
  driveSingleMotor(pins_.rightIn1, pins_.rightIn2, clampPwm(rightPwm));
}

void RvDrv8833Driver::stop() {
  analogWrite(pins_.leftIn1, 0);
  analogWrite(pins_.leftIn2, 0);
  analogWrite(pins_.rightIn1, 0);
  analogWrite(pins_.rightIn2, 0);
}

int16_t RvDrv8833Driver::clampPwm(int16_t pwm) {
  if (pwm > 255) {
    return 255;
  }
  if (pwm < -255) {
    return -255;
  }
  return pwm;
}

// Direction convention assumption: +pwm drives IN1 with PWM and IN2 LOW;
// -pwm drives IN1 LOW and IN2 with PWM.
void RvDrv8833Driver::driveSingleMotor(uint8_t in1Pin, uint8_t in2Pin, int16_t pwm) {
  if (pwm > 0) {
    analogWrite(in1Pin, pwm);
    analogWrite(in2Pin, 0);
    return;
  }

  if (pwm < 0) {
    analogWrite(in1Pin, 0);
    analogWrite(in2Pin, -pwm);
    return;
  }

  analogWrite(in1Pin, 0);
  analogWrite(in2Pin, 0);
}
