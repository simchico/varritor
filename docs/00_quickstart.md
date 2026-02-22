# Quickstart (Arduino IDE-first)

This repository is organized for Arduino IDE as the primary local development path.

## 1) Clone and open
1. Clone this repository locally.
2. Start Arduino IDE (2.x recommended).
3. Open a sketch directly from `arduino/sketches/<sketch_name>/<sketch_name>.ino`.

## 2) Set Sketchbook location
Set Arduino IDE **Sketchbook location** to this repository's `arduino/` directory:

- **Arduino IDE** → **Settings/Preferences** → **Sketchbook location**
- Set to: `<repo-root>/arduino`

Why: this allows the IDE to resolve internal libraries under `arduino/libraries/` without extra configuration.

## 3) Select board and port
1. Tools → Board → **Arduino AVR Boards** → **Arduino Nano**.
2. Tools → Processor → choose the correct bootloader variant for your board.
3. Tools → Port → select the connected Nano serial port.

## 4) Build and upload from IDE
- Use **Verify** to compile.
- Use **Upload** for hardware testing.

## 5) CI is source of truth
Local IDE compile is convenient, but CI gates in `.github/workflows/ci.yml` are authoritative for formatting and compile checks.
