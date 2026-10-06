#ifndef MediumNumbers_h
#define MediumNumbers_h

#include <Arduino.h>
#include <LiquidCrystalFast.h>

// Seven-segment style digits 2 rows high and 3 columns wide
class MediumNumbers
{
  public:
    MediumNumbers(LiquidCrystalFast &lcd);
    void Init();
    void PrintDigit(byte digit, byte col, byte row);  // col, row 0-based, top left corner
  private:
    LiquidCrystalFast *_lcd;
};

#endif
