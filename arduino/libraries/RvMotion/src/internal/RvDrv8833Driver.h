#ifndef RV_DRV8833_DRIVER_H
#define RV_DRV8833_DRIVER_H

#include <Arduino.h>

struct RvDrv8833Pins {
  uint8_t leftIn1;
  uint8_t leftIn2;
  uint8_t rightIn1;
  uint8_t rightIn2;
};

class RvDrv8833Driver {
public:
  explicit RvDrv8833Driver(const RvDrv8833Pins &pins);

  void begin();
  void drive(int16_t leftPwm, int16_t rightPwm);
  void stop();

private:
  static int16_t clampPwm(int16_t pwm);
  static void driveSingleMotor(uint8_t in1Pin, uint8_t in2Pin, int16_t pwm);

  RvDrv8833Pins pins_;
};

#endif
