# Motor Smoke Test (Stage 0, Motion v0)

## Objective
Verify that minimal open-loop motion behavior is safe and repeatable:
- Boot state is STOP.
- Commanded PWM causes expected direction of wheel motion.
- STOP command halts both motors.
- Timeout behavior halts motors when commands stop.

## Preconditions
- Arduino Nano wired to DRV8833 and motors.
- Stable power source for logic and motor rails.
- A dedicated test sketch exists under `arduino/sketches/tests/<name>/` implementing Motion v0 API calls.
- Robot is physically lifted or wheels are free to spin safely.

## Procedure
1. **Boot safety check**
   - Power cycle Nano with test sketch loaded.
   - Observe motors for 5 seconds.
   - Expected: no motor movement.

2. **Forward PWM check**
   - Issue moderate forward command (example: `left=120`, `right=120`).
   - Expected: both wheels rotate forward consistently.

3. **Reverse PWM check**
   - Issue moderate reverse command (example: `left=-120`, `right=-120`).
   - Expected: both wheels rotate reverse consistently.

4. **Differential turn check**
   - Issue differential command (example: `left=120`, `right=-120`).
   - Expected: wheels spin opposite directions (in-place turn behavior).

5. **Explicit stop check**
   - Call `stop()`.
   - Expected: both wheels stop promptly.

6. **Timeout check**
   - Issue motion command once, then stop sending new commands.
   - Wait longer than `MOTION_CMD_TIMEOUT_MS` while `update(now_ms)` continues running.
   - Expected: motion halts automatically and health reports timeout-related status.

## Acceptance Criteria (Pass/Fail)
Pass only if **all** are true:
- No motion at boot.
- Commands produce expected wheel direction for forward/reverse/differential cases.
- Explicit stop always halts motors.
- Timeout always halts motors when command stream ceases.
- No unexpected spin-up, jitter, or runaway behavior.

Fail if any criterion is violated. Record observed behavior and wiring/config details before retry.
