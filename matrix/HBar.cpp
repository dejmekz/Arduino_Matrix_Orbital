#include "HBar.h"

// One pixel column pattern per glyph, repeated on all 8 rows:
// slots 0-3 fill 1-4 pixels from the left, slots 4-7 from the right
static const uint8_t horizontalBarChars[8] PROGMEM = {B10000, B11000, B11100, B11110, B00001, B00011, B00111, B01111};

HBar::HBar(LiquidCrystalFast &lcd, byte width)
{
  _lcd = &lcd;
  _width = width;
}

void HBar::Init()
{
  uint8_t tmpChars[8];

  for (byte i = 0; i < 8; i++)
  {
    memset(tmpChars, pgm_read_byte(&horizontalBarChars[i]), sizeof(tmpChars));
    _lcd->createChar(i, tmpChars);
  }
}

void HBar::Draw(byte dir, byte len)
{
  //Bargraph max size is _width chars
  //len range is 0 to _width * 5 pixels
  byte maxLen = _width * 5;

  if (len > maxLen)
    len = maxLen;

  byte hsize = len / 5;     //length of full block
  byte hrest = len % 5;     //rest - part of block - zero is space char - 1 char
  byte hfree = _width - hsize;

  if ((hrest > 0) && (hfree > 0))
  {
    hfree--;
  }

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

  if (c > _lcd->numcols - _width)
  {
    c = _lcd->numcols - _width;
  }

  _lcd->setCursor(c, r);

  WriteChars(255, pos);

  if (rest > 0)
  {
    _lcd->write(rest - 1);
  }

  WriteChars(32, space);
}


void HBar::DrawToLeft(byte pos, byte rest, byte space)
{
  uint8_t c = 0;
  uint8_t r = 0;

  if (Col > _width)
    c = Col - _width;

  if (Row > 0)
    r = Row - 1;

  if (r > 3)
    r = 3;

  if (c > _lcd->numcols - _width)
  {
    c = _lcd->numcols - _width;
  }

  _lcd->setCursor(c, r);

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
