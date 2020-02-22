#include "VBar.h"

VBar::VBar(LiquidCrystalFast &lcd)
{
  _lcd = &lcd;
}

void VBar::Init(byte chr)
{
  uint8_t tmpChars[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (byte i = 0; i < 8; i++)
  {
    for (byte x = 0; x < 8; x++)
    {
      if (x <= i)
      {
        tmpChars[7 - x] = chr;
      }
      else
      {
        tmpChars[7 - x] = 0x0;
      }
    }
    _lcd->createChar(i, tmpChars);
  }
}

void VBar::Draw(byte col, byte len)
{
  uint8_t Y = 0;

  if (col > 0)
    Y = col - 1;

  len++;

  if (len > 32)
    len = 32;

  byte hsize = len / 8;
  byte hrest = len % 8;
  byte hfree = 4 - hsize;

  byte x = 3;
  if (hsize > 0)
  {
    for (byte r = 0; r < hsize; r++)
    {
      _lcd->setCursor(Y, x);
      _lcd->write(0x07);
      x--;
    }
  }

  if (hrest > 1)
  {
    _lcd->setCursor(Y, x);
    _lcd->write(hrest - 2);
    x--;
    hfree--;
  }

  if (hfree ==  0)
    return;

  for (byte r = 0; r < hfree; r++)
  {
    _lcd->setCursor(Y, x);
    _lcd->write(0x20);
    x--;
  }
}
