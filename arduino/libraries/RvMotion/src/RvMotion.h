#ifndef RV_MOTION_H
#define RV_MOTION_H

#include <Arduino.h>

#include "internal/RvDrv8833Driver.h"

enum RvMotionHealth {
  RV_MOTION_HEALTH_OK = 0,
  RV_MOTION_HEALTH_TIMEOUT_STOP = 1,
};

enum RvMotionState {
  RV_MOTION_STATE_STOPPED = 0,
  RV_MOTION_STATE_DRIVING = 1,
};

struct RvMotionConfig {
  RvDrv8833Pins pins;
  uint32_t commandTimeoutMs;
};

class RvMotion {
public:
  explicit RvMotion(const RvMotionConfig &config);

  void init(uint32_t nowMs);
  void setPWM(int16_t leftPwm, int16_t rightPwm, uint32_t nowMs);
  void stop();
  void update(uint32_t nowMs);

  RvMotionHealth getHealth() const;
  RvMotionState getState() const;
  int16_t getLeftPwm() const;
  int16_t getRightPwm() const;

private:
  static int16_t clampPwm(int16_t pwm);

  RvMotionConfig config_;
  RvDrv8833Driver driver_;
  uint32_t lastCommandMs_;
  int16_t leftPwm_;
  int16_t rightPwm_;
  RvMotionHealth health_;
  RvMotionState state_;
};

#endif
