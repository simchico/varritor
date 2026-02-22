#include <Arduino.h>
#include <RvMotion.h>

// DRV8833 wiring (edit for your board)
static const uint8_t LEFT_IN1_PIN = 5;
static const uint8_t LEFT_IN2_PIN = 6;
static const uint8_t RIGHT_IN1_PIN = 9;
static const uint8_t RIGHT_IN2_PIN = 10;

static const uint32_t MOTION_CMD_TIMEOUT_MS = 1200;
static const uint32_t STEP_DURATION_MS = 1500;

RvMotionConfig motionConfig = {
    {LEFT_IN1_PIN, LEFT_IN2_PIN, RIGHT_IN1_PIN, RIGHT_IN2_PIN},
    MOTION_CMD_TIMEOUT_MS,
};
RvMotion motion(motionConfig);

uint8_t activeStep = 0;
uint32_t stepStartMs = 0;
uint32_t lastStatusPrintMs = 0;

void applyStep(uint8_t step, uint32_t nowMs) {
  switch (step) {
  case 0:
    Serial.println(F("Step 0: FORWARD"));
    motion.setPWM(140, 140, nowMs);
    break;
  case 1:
    Serial.println(F("Step 1: STOP"));
    motion.stop();
    break;
  case 2:
    Serial.println(F("Step 2: REVERSE"));
    motion.setPWM(-140, -140, nowMs);
    break;
  case 3:
    Serial.println(F("Step 3: STOP"));
    motion.stop();
    break;
  case 4:
    Serial.println(F("Step 4: ROTATE"));
    motion.setPWM(140, -140, nowMs);
    break;
  default:
    Serial.println(F("Step 5: STOP (hold)"));
    motion.stop();
    break;
  }
}

void printStatus(uint32_t nowMs) {
  Serial.print(F("t="));
  Serial.print(nowMs);
  Serial.print(F(" step="));
  Serial.print(activeStep);
  Serial.print(F(" state="));
  Serial.print(motion.getState() == RV_MOTION_STATE_DRIVING ? F("DRIVING") : F("STOPPED"));
  Serial.print(F(" health="));
  Serial.print(motion.getHealth() == RV_MOTION_HEALTH_OK ? F("OK") : F("TIMEOUT_STOP"));
  Serial.print(F(" pwmL="));
  Serial.print(motion.getLeftPwm());
  Serial.print(F(" pwmR="));
  Serial.println(motion.getRightPwm());
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  const uint32_t nowMs = millis();
  motion.init(nowMs);
  stepStartMs = nowMs;

  Serial.println(F("Motor smoke test start"));
  applyStep(activeStep, nowMs);
}

void loop() {
  const uint32_t nowMs = millis();

  motion.update(nowMs);

  if ((nowMs - stepStartMs) >= STEP_DURATION_MS && activeStep < 5) {
    activeStep++;
    stepStartMs = nowMs;
    applyStep(activeStep, nowMs);
  }

  if ((nowMs - lastStatusPrintMs) >= 200) {
    lastStatusPrintMs = nowMs;
    printStatus(nowMs);
  }
}
