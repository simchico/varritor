# AGENTS.md — Development Guidelines (Robot Vacuum MCU)

These rules apply to any contribution (human or Codex) in this repository.
If a requested change conflicts with these rules, adjust the plan, not the standards.

## 0) Scope
This repo contains **code only** for a hobby robot vacuum built in **testable stages**, using:
- **Arduino Nano** + Arduino IDE (primary early target)
- **ESP32-S3** (later, dedicated to LiDAR/SLAM)
- **UART** between MCUs
- Text-based **CSV protocol** over UART (debug-friendly)

Documentation lives in `docs/`. Hardware files are out of scope for this repository.

## 1) How to run (local + CI reference)
Local development is Arduino IDE-first, but CI is the source of truth.

Format (files you touched):
- `clang-format -i <file1> <file2> ...`

Format check (CI style):
- `clang-format --dry-run --Werror <files>`

Compile (CI style; exact commands live in `.github/workflows/`):
- Arduino Nano: compile all sketches under `arduino/sketches/**` for `arduino:avr:nano`
- ESP32-S3: only when its sketches exist and are explicitly enabled in CI

If a command is missing, add it to CI rather than relying on undocumented local steps.

## 2) Non-negotiables
1) Keep main branch green
- Every PR must compile and pass CI checks.
- No “fix later”. If CI fails, stop and fix.

2) One PR = one testable capability
- Each PR delivers one small, meaningful, verifiable improvement.
- No unrelated refactors, renames, or “while I was here” changes (YAGNI).

3) Preserve the Black Box architecture
- Behavior / Navigation must never touch pins, PWM, interrupts, or hardware drivers.
- Consumers use only public interfaces (`*_api.h`) and public message formats (protocol).
- Internals can change; contracts must remain stable or be explicitly versioned.

4) Safety first
- Motors must default to **STOP** on boot.
- If commands stop arriving or parsing fails, Motion must **STOP** (timeout).
- No change may allow motors to start unexpectedly due to boot noise, serial noise, or parsing desync.

5) Arduino Nano constraints are real
- Avoid dynamic allocation in the hot path: no `new/delete`, no `malloc/free`, avoid `String`.
- Prefer fixed buffers (`char[]`), POD structs, ring buffers, bounded parsing.
- Prefer simple C++ (no exceptions, no RTTI assumptions, no heavy templates).

## 3) Repository layout (source of truth)
Top-level:
- `docs/` — PRDs, ICDs, ADRs, test procedures/logs
- `arduino/` — sketches + internal Arduino libraries
- `third_party/` — vendored external code (copied in, pinned, with license)

Arduino code:
- `arduino/sketches/` — applications (upload targets)
- `arduino/libraries/` — internal libraries (“boxes”) in Arduino library format

Rules:
- Each sketch folder contains a `.ino` with the same name as the folder.
- Internal libraries expose a small public surface and hide internals behind it.
- Do not run formatting on `third_party/` unless explicitly requested.

## 4) Architecture (Black Box “boxes”)
Boxes (conceptual modules), implemented as libraries:
- **Common**: units, timebase, small utilities
- **Protocol**: UART CSV framing/parsing, message codecs, versioning
- **Motion**: motor driver + encoders + PID + odometry + safety watchdog (implements Motion API)
- **Sensors**: IMU + bumpers/cliff (later) normalized to a Sensors API
- **Behavior**: state machine / coverage logic (uses Motion API + Sensors API only)
- **LiDAR / SLAM** (later): Camsense X1 parsing + scan matching + mapping on ESP32-S3

Dependency rules:
- Behavior depends only on Motion API + Sensors API (never on drivers).
- Protocol must not depend on Motion/Sensors/Behavior logic (only on common types).
- Drivers must not depend on Behavior.

## 5) Contracts and versioning (do not break silently)
Contracts are “source of truth” and must be stable:
- Motion contract: `*_api.h` + documented ICD in `docs/icd/`
- Sensors contract: `*_api.h` + documented ICD in `docs/icd/`
- UART protocol: `docs/protocol/` + shared message definitions

Rules:
- Units must be consistent across the system (pick one and keep it everywhere).
  - Recommended: `mm`, `mrad`, `mm/s`, `mrad/s`, timestamps in `ms`.
- Any breaking change requires:
  - protocol/message version bump
  - explicit ADR in `docs/adr/` describing the change and migration
- If you change a contract, you must update:
  - the ICD documentation
  - at least one smoke/regression test sketch

## 6) UART CSV protocol (debug-first, robust, bounded)
We use **line-based CSV**, one message per line:
- ASCII, comma-separated, terminated by `\n`
- `TYPE` (uppercase token) first field
- `VER` (int) second field
- `T_MS` (int) must be present for all commands/telemetry
- A checksum field is **mandatory** (simple XOR8 or CRC8) unless the PR explicitly justifies disabling it

Hard limits (must be enforced by the parser):
- Maximum line length: **128 bytes** (including `\n`)
- Maximum fields per line: **16**
- Lines violating limits are discarded without side effects

Parsing rules:
- Parser must tolerate partial lines and garbage bytes (use a ring buffer).
- If a line is invalid: discard it and continue searching for the next `\n`.
- Never execute motion commands from a partially parsed line.
- Always clamp/sanitize inputs (speed limits, numeric parsing, field count checks).

Safety rule:
- If no valid motion command arrives within `MOTION_CMD_TIMEOUT_MS`, Motion must STOP.

Protocol documentation:
- Exact message formats live in `docs/protocol/`.
- Code and docs must match.

## 7) Coding principles (practical application)
We follow these principles with MCU realism:
- **KISS**: simplest thing that works and is testable on hardware.
- **DRY**: factor only after duplication exists (2+ uses).
- **YAGNI**: do not build “frameworks” for future steps.
- **SOLID (lightweight)**:
  - SRP: one module, one responsibility
  - interface boundaries via `*_api.h` and protocol messages
- **Law of Demeter**: modules talk via APIs, not internal fields of other modules.

## 8) Formatting and linting (enforced now)
- `clang-format` is mandatory.
- CI must run formatting check.
- Avoid unrelated mass reformatting in PRs.
- Exclude `third_party/` from formatting checks unless explicitly requested.

## 9) Vendoring third-party code
External dependencies must be **copied** into `third_party/` (vendored), not pulled at build time.

Rules:
- Record upstream source (URL + commit/tag) and license in `third_party/README.md`.
- Do not modify vendored code unless necessary; if modified, document changes.
- Prefer permissive licenses (MIT/Apache/BSD) when possible.

## 10) Tests and verification (staged, hardware-aware)
We use two kinds of verification:
1) **CI gates (required)**
- Compile sketches (Arduino Nano at minimum)
- clang-format check
- Lightweight lint/static checks only if reliable for Arduino code

2) **Hardware smoke tests (required as docs or test sketches)**
- Every meaningful capability must have a reproducible procedure in `docs/tests/`.
- Prefer dedicated smoke-test sketches in `arduino/sketches/tests/<name>/`.

Policy:
- Every bug fix should add a reproduction test or deterministic validation step.

## 11) CI requirements (GitHub Actions)
PRs must pass CI. Minimum CI gates:
- Compile all sketches under `arduino/sketches/**` for `arduino:avr:nano`
- clang-format check (excluding `third_party/`)
- Optional: lint checks (only those that run reliably in CI)

Agents must not assume local-only verification is sufficient.

## 12) Pull Requests and commit standardization
We use PRs for all changes.

### Conventional Commits
Commit messages must follow Conventional Commits:
- `feat: ...` new capability
- `fix: ...` bug fix
- `refactor: ...` non-functional changes
- `docs: ...` documentation only
- `test: ...` tests only
- `chore: ...` tooling/maintenance

PR requirements:
- Clear scope and rationale
- Explicit “How to test” steps
- Link to PRD/issue/ADR when applicable
- No unrelated changes

## 13) Review severity levels (for Codex and humans)
Use these severities when reviewing PRs:

P0 — must fix before merge
- Safety violation (motors can run unexpectedly)
- Contract/protocol break without versioning + docs
- CI failure
- Parser can execute commands from invalid/partial input
- Unbounded buffers / potential overflow on Nano

P1 — should fix before merge
- Black box boundary violation (Behavior touches drivers)
- Use of `String` / dynamic allocation in Nano hot path
- Missing/unclear test steps for a meaningful change
- Protocol docs and code mismatch

P2 — optional / follow-up
- Minor style improvements, non-blocking refactors, doc polish

## 14) Definition of Done (per PR)
A PR is “done” only if:
- CI passes (compile + format + required checks)
- Contracts preserved (or versioned + docs updated + ADR if breaking)
- A test exists:
  - smoke-test sketch and/or
  - documented hardware test steps in `docs/tests/`
- Safety behavior is preserved (STOP default, command timeout, input sanitization)
- Code is small, readable, and fits Nano constraints

## 15) Agent workflow (Codex or human)
1) Plan
- Read relevant PRD/ICD/ADR in `docs/`.
- Identify exact files to touch and tests to update/add.

2) Implement minimal
- Make the smallest change that satisfies the capability and tests.
- Avoid mass formatting or unrelated cleanups.

3) Verify
- Ensure CI will pass (compile + format).
- Add/adjust smoke tests and documentation as required.

4) Self-audit
- Black box boundaries intact
- Safety preserved
- Parser bounded and robust

## 16) Explicit “DO NOT” list
- Do not change public contracts without versioning + docs updates.
- Do not introduce dynamic allocation / `String` usage on Nano without strong justification and constraints.
- Do not mix Behavior logic into drivers or protocol code.
- Do not add complexity “for later” without an approved PRD item.
- Do not ship PRs that rely on manual, undocumented testing.
