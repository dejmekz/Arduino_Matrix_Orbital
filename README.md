Matrix
======

Matrix Orbital Emulated Arduino HD44780 Device for Arduino Nano V3.0

Breadboard

![Breadboard](http://s10.postimg.org/roe0sj1ll/tisplay_breadboard.png)

Circuit Diagram

![Circuit Diagram](http://s23.postimg.org/ijjwnjtqz/tisplay_circuit.png)

Building
--------

### With Docker (no local toolchain needed)

`build.sh` compiles in a Docker container with PlatformIO and the AVR toolchain
(`docker/Dockerfile`); it also works on Apple Silicon without Rosetta.

    ./build.sh                       # Arduino Nano V3.0 (default)
    ./build.sh miniatmega168         # Arduino Mini (ATmega168)
    ./build.sh all                   # all environments
    ./build.sh clean

The firmware ends up in `.pio/build/<env>/firmware.hex`.

Upload over the serial bootloader runs `avrdude` on the host, because Docker
Desktop on macOS cannot pass USB serial ports to containers
(macOS: `brew install avrdude`):

    ./build.sh upload miniatmega168                        # single USB serial port is detected
    ./build.sh upload miniatmega168 /dev/cu.usbserial-1410 # or give the port

| Environment        | Board                           | MCU        | Bootloader baud |
|--------------------|---------------------------------|------------|-----------------|
| `nanoatmega328`    | Nano V3.0, old bootloader       | ATmega328P | 57600           |
| `nanoatmega328new` | Nano V3.0, Optiboot bootloader  | ATmega328P | 115200          |
| `miniatmega168`    | Arduino Mini / ATmega168        | ATmega168  | 19200           |

### With a local PlatformIO

    pio run -e miniatmega168
    pio run -e miniatmega168 -t upload

### Library

The [LiquidCrystalFast](https://www.pjrc.com/teensy/td_libs_LiquidCrystal.html)
library (40x4 support with two enable lines) is not in the PlatformIO registry,
so a copy is kept in `lib/LiquidCrystalFast`.

The sketch can still be opened in the Arduino IDE from `matrix/matrix.ino`;
there LiquidCrystalFast has to be installed separately (it ships with
Teensyduino, or copy `lib/LiquidCrystalFast` into your Arduino `libraries` folder).
