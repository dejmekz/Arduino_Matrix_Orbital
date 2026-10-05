#ifndef VBar_h
#define VBar_h

#include <Arduino.h>
#include <LiquidCrystalFast.h>

class VBar
{
  public:

    VBar(LiquidCrystalFast &lcd);
    void Init(byte chr);
    void Draw(byte col, byte len);
  private:
    LiquidCrystalFast *_lcd;
};

#endif
