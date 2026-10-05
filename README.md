Matrix
======

Matrix Orbital Emulated Arduino HD44780 Device for Arduino Nano V3.0

Breadboard

![Breadboard](http://s10.postimg.org/roe0sj1ll/tisplay_breadboard.png)

Circuit Diagram

![Circuit Diagram](http://s23.postimg.org/ijjwnjtqz/tisplay_circuit.png)

Building
--------

The project builds with [PlatformIO](https://platformio.org/):

    pio run                          # Arduino Nano V3.0 (default)
    pio run -e nanoatmega328new      # Nano with the new bootloader
    pio run -e miniatmega168         # Arduino Mini (ATmega168)
    pio run -t upload

The [LiquidCrystalFast](https://www.pjrc.com/teensy/td_libs_LiquidCrystal.html)
library (40x4 support with two enable lines) is not in the PlatformIO registry,
so a copy is kept in `lib/LiquidCrystalFast`.

The sketch can still be opened in the Arduino IDE from `matrix/matrix.ino`;
there LiquidCrystalFast has to be installed separately (it ships with
Teensyduino, or copy `lib/LiquidCrystalFast` into your Arduino `libraries` folder).
