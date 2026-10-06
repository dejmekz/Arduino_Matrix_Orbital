Matrix
======

Matrix Orbital Emulated Arduino HD44780 Device for Arduino Nano V3.0

Version 2.0 - Matrix Orbital LCD4041 emulation on a 40x4 HD44780 display
(two controllers), for Arduino Nano V3.0 (ATmega328P) and Arduino Mini (ATmega168).

What is new in 2.0:

- fixed command handling (GPO, goto, bar graphs, character mapping) and
  cursor handling across both controllers of the 40x4 display
- startup screen stored in EEPROM (`0xFE 0x40`), defaults on a blank EEPROM
- medium digits (`0xFE 0x6D` / `0xFE 0x6F`), 2 rows x 3 columns
- backlight PWM on D10 with saved brightness and auto-off timer,
  limited to 50% for USB power
- glyph tables in flash (static RAM 678 -> 258 bytes)
- PlatformIO project with a Docker build and avrdude upload script

Breadboard

![Breadboard](http://s10.postimg.org/roe0sj1ll/tisplay_breadboard.png)

Circuit Diagram

![Circuit Diagram](http://s23.postimg.org/ijjwnjtqz/tisplay_circuit.png)

Hardware
--------

| Arduino pin | Use                                                        |
|-------------|------------------------------------------------------------|
| D7          | LCD RS                                                     |
| D9          | LCD R/W                                                    |
| D8          | LCD E1 (rows 1-2)                                          |
| D6          | LCD E2 (rows 3-4)                                          |
| D3, D2, D5, D4 | LCD D4, D5, D6, D7                                      |
| D10         | Backlight PWM, via MOSFET - see Backlight below            |
| D13         | GPO 1 (on-board LED)                                       |

Contrast is set with a trimmer on the LCD V0 pin; contrast commands are ignored.

### Backlight

The LED backlight is switched on its cathode (K) by an N-MOSFET driven with
PWM from D10. A series resistor on the anode (A) limits the current.

```
              +5V
               │
            [R_BL]    series resistor 4.7 Ω / 2 W
               │
             LCD A
             LCD K    LED backlight
               │
               │ D
  D10 ─[220Ω]─┬┤ G    IRF3205 (G-D-S)
              ││ S
           [10kΩ]│
              │  │
  GND ────────┴──┴─── GND, own wire to the power supply
```

- **MOSFET**: IRF3205. It is not a logic-level part, but at a 5 V gate it
  switches 0.5 A easily. A logic-level MOSFET (IRLZ44N, IRL3705N, AO3400)
  works as well. Check that the drain-source voltage at full brightness is
  below ~0.1 V.
- **220 Ω** gate resistor limits the pin current when charging the gate;
  **10 kΩ** pull-down keeps the backlight off during reset and bootloader.
- **R_BL = 4.7 Ω / 2 W**: without it the backlight of the PM4040-1 rev B module
  draws about 2.2 A at 5.2 V; with it about 0.53 A at brightness 255. The
  resistor then dissipates `0.53² x 4.7 ≈ 1.3 W` and runs warm, so keep it
  away from the LCD glass and give it air (a 3 W part gives more margin).
  For another module, size it for ~0.5 A: `R_BL = (5.2 V - V_LED) / 0.5 A`,
  with the wattage well above `R_BL x I²`.
- **Power**: the backlight current must not flow through thin wires or the
  LCD GND pin. A voltage drop on VDD shifts the contrast (with 0.65 V of drop
  all character cells turned dark when the backlight was dimmed). Run the
  MOSFET source and the resistor with their own wires to a 5 V supply that
  can deliver at least 1 A; a USB-serial adapter cannot.

Measured on the PM4040-1 rev B at 5.2 V by PWM duty (whole board, the logic
draws 0.035 A); with the default limit, brightness 255 gives the 128 column:

| Brightness | 255     | 128     | 64      | 16      | 0       |
|------------|---------|---------|---------|---------|---------|
| Current    | 0.561 A | 0.293 A | 0.161 A | 0.061 A | 0.035 A |

| Command            | Function                                         |
|--------------------|--------------------------------------------------|
| `0xFE 0x42 [min]`  | backlight on, off again after `min` minutes (0 = stay on) |
| `0xFE 0x46`        | backlight off                                    |
| `0xFE 0x99 [level]`| set brightness 0-255                             |
| `0xFE 0x98 [level]`| set and save brightness, restored at power up    |

**Brightness limit**: so the board can run from a USB port (500 mA), the
firmware scales the brightness 0-255 sent by the host to a PWM duty of at most
50% (`MAX_BRIGHTNESS = 128` in `matrix.ino`). Brightness 255 then draws about
0.29 A for the whole board. With a stronger 5 V supply, raise `MAX_BRIGHTNESS`
up to 255.

Startup screen: send `0xFE 0x40` followed by 160 characters (4 rows x 40) to
store a screen that is shown on the next power up. 160 spaces restore the
built-in screen.

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
