#ifndef HBar_h
#define HBar_h

#include <Arduino.h>
#include <LiquidCrystalFast.h>

class HBar
{
  public:
    byte Col;
    byte Row;

    HBar(LiquidCrystalFast &lcd, byte width = 20);  // width of the bar field in chars
    void Init();
    void Draw(byte dir, byte len);
  private:
    LiquidCrystalFast *_lcd;
    byte _width;

    void WriteChars(byte chr, byte len);
    void DrawToRight(byte pos, byte rest, byte space);
    void DrawToLeft(byte pos, byte rest, byte space);
};

#endif
