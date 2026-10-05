#include "BigNumbers.h"

// Segment glyphs, loaded into CGRAM slots 1-7
static const uint8_t bigNumberChars[7][8] PROGMEM = {
  {
    B00000,
    B00000,
    B00000,
    B00000,
    B00011,
    B01111,
    B01111,
    B11111
  }, {
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
    B11111
  }, {
    B00000,
    B00000,
    B00000,
    B00000,
    B11000,
    B11110,
    B11110,
    B11111
  }, {
    B11111,
    B01111,
    B01111,
    B00011,
    B00000,
    B00000,
    B00000,
    B00000
  }, {
    B11111,
    B11110,
    B11110,
    B11000,
    B00000,
    B00000,
    B00000,
    B00000
  }, {
    B11111,
    B11111,
    B11111,
    B11111,
    B00000,
    B00000,
    B00000,
    B00000
  }, {
    B01110,
    B01110,
    B01110,
    B01110,
    B01100,
    B01000,
    B00000,
    B00000
  }
};

// Each digit is 3 columns x 4 rows of CGRAM slots, 255 = full block, 32 = space
static const uint8_t bigNumbersArray[10][4][3] PROGMEM = {
  {
    // custom 0
    {1, 2, 3},
    {255, 32, 255},
    {255, 32, 255},
    {4, 6, 5}
  }, {
    // custom 1
    {2, 3, 32},
    {32, 255, 32},
    {32, 255, 32},
    {6, 6, 6}
  }, {
    // custom 2
    {1, 2, 3},
    {1, 2, 255},
    {255, 32, 32},
    {4, 6, 6}
  }, {
    // custom 3
    {1, 2, 3},
    {32, 2, 255},
    {32, 32, 255},
    {4, 6, 5}
  }, {
    // custom 4
    {2, 32, 32},
    {255, 2, 2},
    {32, 255, 32},
    {32, 6, 32}
  }, {
    // custom 5
    {2, 2, 2},
    {255, 2, 2},
    {32, 32, 255},
    {6, 6, 5}
  }, {
    // custom 6
    {1, 2, 3},
    {255, 2, 3},
    {255, 32, 255},
    {4, 6, 5}
  }, {
    // custom 7
    {2, 2, 2},
    {32, 2, 255},
    {32, 255, 32},
    {32, 6, 32}
  }, {
    // custom 8
    {1, 2, 3},
    {255, 2, 255},
    {255, 32, 255},
    {4, 6, 5}
  }, {
    // custom 9
    {1, 2, 3},
    {255, 32, 255},
    {4, 6, 255},
    {32, 32, 6}
  }
};

BigNumbers::BigNumbers(LiquidCrystalFast &lcd)
{
  _lcd = &lcd;
}

void BigNumbers::Init()
{
  uint8_t glyph[8];

  for (byte i = 0; i < 7; i++)
  {
    memcpy_P(glyph, bigNumberChars[i], sizeof(glyph));
    _lcd->createChar(i + 1, glyph);
  }
}

void BigNumbers::PrintBigCharOnPosition(byte digit, byte position)
{
  if (digit > 9) return;
  if (position > 9) return;

  GenericDigitPrint(digit, position * 4);
}

void BigNumbers::PrintBigChar(byte digit, byte col)
{
  if (digit > 9) return;
  GenericDigitPrint(digit, col);
}

void BigNumbers::GenericDigitPrint(byte digit, byte column)
{
  // print rows
  for (byte y = 0; y < 4; y++)
  {
    _lcd->setCursor(column, y);

    // 3 custom chars per row
    for (byte x = 0; x < 3; x++)
    {
      _lcd->write(pgm_read_byte(&bigNumbersArray[digit][y][x]));
    }
  }
}
