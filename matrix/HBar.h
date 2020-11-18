#ifndef HBar_h
#define HBar_h

#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"    // for digitalRead, digitalWrite, pinMode, delayMicroseconds
#else
#include "WProgram.h"
#endif
#include <LiquidCrystalFast.h>

class HBar
{
  public:
    byte Col;
    byte Row;

    HBar(LiquidCrystalFast &lcd);
    void Init();
    void Draw(byte dir, byte len);
  private:
    LiquidCrystalFast *_lcd;

    uint8_t horizontalBarChars[8] = {B10000, B11000, B11100, B11110, B00001, B00011, B00111, B01111};

    /*
        uint8_t horizontalBarChars[8][8] = {
          {
            B10000,
            B10000,
            B10000,
            B10000,
            B10000,
            B10000,
            B10000,
            B10000
          }, {
            B11000,
            B11000,
            B11000,
            B11000,
            B11000,
            B11000,
            B11000,
            B11000
          }, {
            B11100,
            B11100,
            B11100,
            B11100,
            B11100,
            B11100,
            B11100,
            B11100
          }, {
            B11110,
            B11110,
            B11110,
            B11110,
            B11110,
            B11110,
            B11110,
            B11110
          }, {
            B00001,
            B00001,
            B00001,
            B00001,
            B00001,
            B00001,
            B00001,
            B00001
          }, {
            B00011,
            B00011,
            B00011,
            B00011,
            B00011,
            B00011,
            B00011,
            B00011
          }, {
            B00111,
            B00111,
            B00111,
            B00111,
            B00111,
            B00111,
            B00111,
            B00111
          }, {
            B01111,
            B01111,
            B01111,
            B01111,
            B01111,
            B01111,
            B01111,
            B01111
          }
        };
    */
    void WriteChars(byte chr, byte len);
    void DrawToRight(byte pos, byte rest, byte space);
    void DrawToLeft(byte pos, byte rest, byte space);
};

#endif
