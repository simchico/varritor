# Motion ICD v0 (Minimal, Open-Loop)

## Status
- Version: **v0**
- Scope: minimal open-loop motion control for DRV8833 via PWM (no encoders, no PID, no odometry)
- Target: Arduino Nano

## Purpose
Define the initial Motion module contract used by higher-level modules and smoke tests while keeping implementation simple and safe.

## Public Motion API (v0)
The Motion box exposes four operations:

1. `setPWM(int16_t left_pwm, int16_t right_pwm)`
   - Sets raw wheel PWM commands.
   - Input range must be clamped to implementation limits (recommended `[-255, 255]`).
   - Sign convention must be documented in implementation (`+` forward, `-` reverse).

2. `stop()`
   - Immediately commands both motors to zero output.
   - Must be safe to call repeatedly.

3. `update(uint32_t now_ms)`
   - Called from loop with monotonic milliseconds.
   - Enforces command timeout and any safety interlocks.

4. `health()`
   - Returns basic status flags/codes for diagnostics (e.g., `OK`, `TIMEOUT_STOP`, `CONFIG_ERROR`).

## Safety Requirements (Normative)
1. **Boot default STOP**
   - On initialization, motor outputs must be set to zero.

2. **Timeout STOP**
   - If no valid motion command has been accepted within `MOTION_CMD_TIMEOUT_MS`, Motion must transition to STOP.

3. **Input sanitization**
   - Out-of-range PWM values must be clamped.
   - Invalid command input must not produce motion.

4. **Fail-safe behavior**
   - Any internal fault state must resolve to STOP output.

## Out of Scope for v0
- Closed-loop speed control (PID)
- Encoders and odometry
- Trajectory tracking
- UART protocol details (defined separately)

## Verification Link
- Hardware smoke test procedure: `docs/tests/motor_smoke_test.md`
