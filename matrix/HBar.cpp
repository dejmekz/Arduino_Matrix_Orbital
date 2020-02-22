#include "HBar.h"

HBar::HBar(LiquidCrystalFast &lcd)
{
  _lcd = &lcd;
}

void HBar::Init()
{
  uint8_t tmpChars[8] = {0,0,0,0,0,0,0,0};
  
  for (int i = 0; i < 8; i++)
  {
    for (int x = 0; x < 8; x++)
    {
      tmpChars[x] = horizontalBarChars[i];
    }
    _lcd->createChar(i, tmpChars);
  }
}

void HBar::Draw(byte dir, byte len)
{
  //Bargraph max size is 20 chars
  //len range is 0 to 100

  if (len < 0)
    len = 0;

  if (len > 100)
    len = 100;

  byte hsize = len / 5;     //length of full block
  byte hrest = len % 5;     //rest - part of block - zero is space char - 1 char
  byte hfree = 20 - hsize;  //20 - hsize - rest = 19 - hsize

  //left to right =>
  if (dir == 0)
  {
    DrawToRight(hsize, hrest, hfree);
  }

  //right to left <=
  if (dir == 1)
  {
    DrawToLeft(hsize, hrest, hfree);
  }
}


void HBar::DrawToRight(byte pos, byte rest, byte space)
{

  uint8_t c = 0;
  uint8_t r = 0;

  if (Row > 0)
    r = Row - 1;

  if (r > 3)
    r = 3;

  if (Col > 0)
  {
    c = Col - 1;
  }

  if (c > 20)
  {
    c = 20;
  }

  _lcd->setCursor(c, r);

  WriteChars(255, pos);

  if (rest > 0)
  {
    _lcd->write(rest - 1);
    space--;
  }

  WriteChars(32, space);
}


void HBar::DrawToLeft(byte pos, byte rest, byte space)
{
  uint8_t c = 0;
  uint8_t r = 0;

  if (Col > 20)
    c = Col - 20;

  if (Row > 0)
    r = Row - 1;

  if (r > 3)
    r = 3;

  if (c > 39)
  {
    c = 39;
  }

  _lcd->setCursor(c, r);

  if (rest > 0)
  {
    space--;
  }

  WriteChars(32, space);

  if (rest > 0)
  {
    _lcd->write(rest + 3);
  }

  WriteChars(255, pos);
}

void HBar::WriteChars(byte chr, byte len)
{
  if (len == 0)
    return;
  
  for (byte i = 0; i < len; i++)
  {
    _lcd->write(chr);
  }
}
