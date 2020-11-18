#ifndef VBar_h
#define VBar_h

#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"    // for digitalRead, digitalWrite, pinMode, delayMicroseconds
#else
#include "WProgram.h"
#endif
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
