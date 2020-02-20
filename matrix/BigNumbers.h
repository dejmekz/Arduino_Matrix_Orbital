#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"    // for digitalRead, digitalWrite, pinMode, delayMicroseconds
#else
#include "WProgram.h"
#endif
#include <LiquidCrystalFast.h>

class BigNumbers
{
  public:
    BigNumbers(LiquidCrystalFast &lcd);
    void Init();
    void PrintBigCharOnPosition(byte digit, byte position);
    void PrintBigChar(byte digit, byte col);
  private:
    LiquidCrystalFast *_lcd;

    byte bigNumberChars[7][8] = {
      {
        B00000,
        B00000,
        B00000,
        B00000,
        B00011,
        B01111,
        B01111,
        B11111
      }, {
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111,
        B11111,
        B11111
      }, {
        B00000,
        B00000,
        B00000,
        B00000,
        B11000,
        B11110,
        B11110,
        B11111
      }, {
        B11111,
        B01111,
        B01111,
        B00011,
        B00000,
        B00000,
        B00000,
        B00000
      }, {
        B11111,
        B11110,
        B11110,
        B11000,
        B00000,
        B00000,
        B00000,
        B00000
      }, {
        B11111,
        B11111,
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000
      }, {
        B01110,
        B01110,
        B01110,
        B01110,
        B01100,
        B01000,
        B00000,
        B00000
      }
    };

    byte bigNumbersArray[10][4][3] = {
      {
        // custom 0
        {1, 2, 3},
        {255, 254, 255},
        {255, 254, 255},
        {4, 6, 5}
      }, {
        // custom 1
        {2, 3, 254},
        {254, 255, 254},
        {254, 255, 254},
        {6, 6, 6}
      }, {
        // custom 2
        {1, 2, 3},
        {1, 2, 255},
        {255, 254, 254},
        {4, 6, 6}
      }, {
        // custom 3
        {1, 2, 3},
        {254, 2, 255},
        {254, 254, 255},
        {4, 6, 5}
      }, {
        // custom 4
        {2, 254, 254},
        {255, 2, 2},
        {254, 255, 254},
        {254, 6, 254}
      }, {
        // custom 5
        {2, 2, 2},
        {255, 2, 2},
        {254, 254, 255},
        {6, 6, 5}
      }, {
        // custom 6
        {1, 2, 3},
        {255, 2, 3},
        {255, 254, 255},
        {4, 6, 5}
      }, {
        // custom 7
        {2, 2, 2},
        {254, 2, 255},
        {254, 255, 254},
        {254, 6, 254}
      }, {
        // custom 8
        {1, 2, 3},
        {255, 2, 255},
        {255, 254, 255},
        {4, 6, 5}
      }, {
        // custom 9
        {1, 2, 3},
        {255, 254, 255},
        {4, 6, 255},
        {254, 254, 6}
      }
    };
    /*
        byte bigPunctuationArray[2][4][3] = {
          {
            // custom colon
            {254, 254, 254},
            {254, 2, 254},
            {254, 6, 254},
            {254, 254, 254}
          }, {
            // custom comma
            {254, 254, 254},
            {254, 254, 254},
            {254, 254, 254},
            {254, 7, 254}
          }
        };
    */

    void GenericDigitPrint(byte digit, byte column);
};
