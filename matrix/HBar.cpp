#include "HBar.h"

HBar::HBar(LiquidCrystalFast &lcd)
{
  _lcd = &lcd;
}

void HBar::Init()
{
  byte tmpChars[8];
  
  for (int i = 0; i < 8; i++)
  {
    for (byte x = 0; x < 8; i++)
    {
      tmpChars[x] = horizontalBarChars[i];
    }
    
    _lcd->createChar(i, tmpChars[i]);
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
  byte hfree = 19 - hsize;  //20 - hsize - rest = 19 - hsize

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
 byte c = Col - 1;
 byte r = Row - 1;

    if (r < 0)
      r = 0;

    if (r > 3)
      r = 3;

    if (c < 0)
    {
      c = 0;
    }

    if (c > 20)
    {
      c = 20;
    }

    _lcd->setCursor(c, r);

    WriteChars(255, pos);

    if (rest == 0)
    {
      _lcd->write(32);
    }
    else
    {
      _lcd->write(rest - 1);
    }

    WriteChars(32, space);
}


void HBar::DrawToLeft(byte pos, byte rest, byte space)
{
 byte c = Col - 21;
 byte r = Row - 1;

    if (r < 0)
      r = 0;

    if (r > 3)
      r = 3;

    if (c < 0)
    {
      c = 0;
    }

    if (c > 39)
    {
      c = 39;
    }

    _lcd->setCursor(c, r);

    WriteChars(32, space);

    if (rest == 0)
    {
      _lcd->write(32);
    }
    else
    {
      _lcd->write(rest + 3);
    }

    WriteChars(255, pos);
  
}

void HBar::WriteChars(byte chr, byte len)
{
    for (byte i = 0; i < len; i++)
    {
      _lcd->write(chr);
    }  
}
