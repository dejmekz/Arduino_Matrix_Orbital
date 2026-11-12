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

// Col and Row are 1-based. Left to right the bar starts at Col, right to left
// it ends at Col. The bar field is _width chars, cut at the display edge.
void HBar::Draw(byte dir, byte len)
{
  byte cols = _lcd->numcols;
  byte c = (Col > 0) ? Col - 1 : 0;
  byte r = (Row > 0) ? Row - 1 : 0;
  byte width;

  if (c >= cols)
    c = cols - 1;

  if (r >= _lcd->numlines)
    r = _lcd->numlines - 1;

  if (dir == 0)
  {
    width = min(_width, (byte)(cols - c));
  }
  else if (dir == 1)
  {
    width = min(_width, (byte)(c + 1));
    c = c + 1 - width;
  }
  else
  {
    return;
  }

  //len range is 0 to width * 5 pixels
  if (len > width * 5)
    len = width * 5;

  byte full = len / 5;      //full blocks
  byte rest = len % 5;      //pixels of the partial block
  byte space = width - full - (rest > 0 ? 1 : 0);

  _lcd->setCursor(c, r);

  if (dir == 0)
  {
    //left to right =>
    WriteChars(255, full);
    if (rest > 0)
      _lcd->write(rest - 1);
    WriteChars(32, space);
  }
  else
  {
    //right to left <=
    WriteChars(32, space);
    if (rest > 0)
      _lcd->write(rest + 3);
    WriteChars(255, full);
  }
}

void HBar::WriteChars(byte chr, byte len)
{
  for (byte i = 0; i < len; i++)
  {
    _lcd->write(chr);
  }
}
