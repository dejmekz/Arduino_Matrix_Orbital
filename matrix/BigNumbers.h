#ifndef BigNumbers_h
#define BigNumbers_h

#include <Arduino.h>
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

    void GenericDigitPrint(byte digit, byte column);
};

#endif
