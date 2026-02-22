#include "RvMotion.h"

RvMotion::RvMotion(const RvMotionConfig &config)
    : config_(config), driver_(config.pins), lastCommandMs_(0), leftPwm_(0), rightPwm_(0),
      health_(RV_MOTION_HEALTH_OK), state_(RV_MOTION_STATE_STOPPED) {}

void RvMotion::init(uint32_t nowMs) {
  driver_.begin();
  leftPwm_ = 0;
  rightPwm_ = 0;
  state_ = RV_MOTION_STATE_STOPPED;
  health_ = RV_MOTION_HEALTH_OK;
  lastCommandMs_ = nowMs;
  driver_.stop();
}

void RvMotion::setPWM(int16_t leftPwm, int16_t rightPwm, uint32_t nowMs) {
  leftPwm_ = clampPwm(leftPwm);
  rightPwm_ = clampPwm(rightPwm);
  lastCommandMs_ = nowMs;
  health_ = RV_MOTION_HEALTH_OK;

  if (leftPwm_ == 0 && rightPwm_ == 0) {
    stop();
    return;
  }

  driver_.drive(leftPwm_, rightPwm_);
  state_ = RV_MOTION_STATE_DRIVING;
}

void RvMotion::stop() {
  leftPwm_ = 0;
  rightPwm_ = 0;
  state_ = RV_MOTION_STATE_STOPPED;
  driver_.stop();
}

void RvMotion::update(uint32_t nowMs) {
  if (state_ == RV_MOTION_STATE_STOPPED) {
    return;
  }

  const uint32_t elapsedMs = nowMs - lastCommandMs_;
  if (elapsedMs >= config_.commandTimeoutMs) {
    stop();
    health_ = RV_MOTION_HEALTH_TIMEOUT_STOP;
  }
}

RvMotionHealth RvMotion::getHealth() const { return health_; }

RvMotionState RvMotion::getState() const { return state_; }

int16_t RvMotion::getLeftPwm() const { return leftPwm_; }

int16_t RvMotion::getRightPwm() const { return rightPwm_; }

int16_t RvMotion::clampPwm(int16_t pwm) {
  if (pwm > 255) {
    return 255;
  }
  if (pwm < -255) {
    return -255;
  }
  return pwm;
}
