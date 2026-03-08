# GPU-Display

A Linux daemon that maps CPU temperature to RGB lighting on the **Sapphire RX 9060 XT** via I2C — because there is no
official software for it on Linux.

At 50 °C the GPU glows blue. At 95 °C it goes red.

---

## How it works

1. Reads CPU temperature from `/sys/class/thermal/thermal_zone0/temp`
2. Smooths it with an exponential moving average filter (α = 0.05, ~4 s effective window)
3. Maps the range **50 °C → 95 °C** to an HSV hue sweep **240° → 0°** (blue → red)
4. Scales the resulting 0–255 RGB value to the hardware range 0–26
5. Writes R/G/B registers to the LED controller over I2C at ~30 fps

### Color scale

| Temperature | Color  |
|-------------|--------|
| 50 °C       | Blue   |
| 60 °C       | Cyan   |
| 70 °C       | Green  |
| 80 °C       | Yellow |
| 90 °C       | Orange |
| 95 °C+      | Red    |

---

## Configuration

Two values are hardcoded in source and can be changed:

**Temperature source** — `src/cpu_temp_sensor.h`

Default is `/sys/class/thermal/thermal_zone0/temp`. Change the default argument in the `CpuTempSensor` constructor to
any sysfs thermal zone on your system.

**GPU I2C address** — `src/gpu_light_controller.cc`

Default bus is `0` (`/dev/i2c-0`), default device address is `0x28` (Sapphire RX 9060 XT). Change `kSapphireAddr` or
pass a different bus number to the `SapphireRX9060XTLedController` constructor in `main.cc`.

---

## Requirements

- Linux with `i2c-dev` kernel module loaded
- Access to `/dev/i2c-*` (add user to `i2c` group, or run as root)
- C++23 compiler (GCC or Clang)
- CMake ≥ 3.21

Load the module:

```sh
modprobe i2c-dev
```

Add yourself to the `i2c` group (takes effect after relogin):

```sh
sudo usermod -aG i2c $USER
```

---

## Build

```sh
# Release
make release
./build/release/gpu-display

# Debug (ASan + UBSan)
make debug
./build/debug/gpu-display
```

---

## Usage

```sh
sudo ./build/release/gpu-display
```

The daemon starts immediately, updates the GPU LEDs in a loop, and turns them off cleanly on `Ctrl+C` / `SIGINT`.

### Run as a systemd service

```ini
[Unit]
Description = GPU RGB temperature display
After = multi-user.target

[Service]
ExecStart = /usr/local/bin/gpu-display
Restart = on-failure

[Install]
WantedBy = multi-user.target
```

---

## I2C register map

Address `0x28` on `/dev/i2c-0` (Sapphire RX 9060 XT).

| Register | Name             | Value                               |
|----------|------------------|-------------------------------------|
| `0x0F`   | External control | `0x00` — take software control      |
| `0x10`   | Mode             | `0x06` — custom color, `0x07` — off |
| `0x1A`   | Red              | 0–26 (hardware units)               |
| `0x1B`   | Green            | 0–26                                |
| `0x1C`   | Blue             | 0–26                                |

> The hardware channel range is **0–26**, not 0–255. Values above 26 wrap back to 0. The driver scales the 8-bit
> software value accordingly.

---

## Project structure

```
src/
├── main.cc                    entry point, color mapping loop
├── cpu_temp_sensor.h/cc       reads sysfs thermal zone
├── gpu_light_controller.h/cc  I2C LED driver
├── temp_filter.h/cc           exponential moving average
└── types.h                    fixed-width type aliases
```

---

## License

Do whatever you want with it.
