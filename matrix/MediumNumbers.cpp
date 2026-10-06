#include "MediumNumbers.h"

// CGRAM slots
enum {
  LEFT_TOP,     // segment f - vertical bar at the right edge of the left column
  RIGHT_TOP,    // segment b - vertical bar at the left edge of the right column
  LEFT_BOTTOM,  // segment e
  RIGHT_BOTTOM, // segment c
  BAR_A,        // middle column, top row: segment a
  BAR_AG,       // middle column, top row: segments a and g
  BAR_G,        // middle column, top row: segment g
  BAR_D         // middle column, bottom row: segment d
};

static const uint8_t mediumNumberChars[8][8] PROGMEM = {
  {B00011, B00011, B00011, B00011, B00011, B00011, B00011, B00011},  // LEFT_TOP
  {B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000},  // RIGHT_TOP
  {B00011, B00011, B00011, B00011, B00011, B00011, B00011, B00000},  // LEFT_BOTTOM
  {B11000, B11000, B11000, B11000, B11000, B11000, B11000, B00000},  // RIGHT_BOTTOM
  {B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000},  // BAR_A
  {B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111},  // BAR_AG
  {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111},  // BAR_G
  {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B00000},  // BAR_D
};

// Segments per digit, bits: a b c d e f g
#define SEG_A 0x40
#define SEG_B 0x20
#define SEG_C 0x10
#define SEG_D 0x08
#define SEG_E 0x04
#define SEG_F 0x02
#define SEG_G 0x01

static const uint8_t digitSegments[10] PROGMEM = {
  0x7E,  // 0: a b c d e f
  0x30,  // 1: b c
  0x6D,  // 2: a b d e g
  0x79,  // 3: a b c d g
  0x33,  // 4: b c f g
  0x5B,  // 5: a c d f g
  0x5F,  // 6: a c d e f g
  0x70,  // 7: a b c
  0x7F,  // 8: all
  0x7B,  // 9: a b c d f g
};

MediumNumbers::MediumNumbers(LiquidCrystalFast &lcd)
{
  _lcd = &lcd;
}

void MediumNumbers::Init()
{
  uint8_t glyph[8];

  for (byte i = 0; i < 8; i++)
  {
    memcpy_P(glyph, mediumNumberChars[i], sizeof(glyph));
    _lcd->createChar(i, glyph);
  }
}

void MediumNumbers::PrintDigit(byte digit, byte col, byte row)
{
  if (digit > 9) return;

  byte s = pgm_read_byte(&digitSegments[digit]);
  byte middle = ' ';

  if ((s & SEG_A) && (s & SEG_G))
    middle = BAR_AG;
  else if (s & SEG_A)
    middle = BAR_A;
  else if (s & SEG_G)
    middle = BAR_G;

  _lcd->setCursor(col, row);
  _lcd->write((s & SEG_F) ? LEFT_TOP : ' ');
  _lcd->write(middle);
  _lcd->write((s & SEG_B) ? RIGHT_TOP : ' ');

  _lcd->setCursor(col, row + 1);
  _lcd->write((s & SEG_E) ? LEFT_BOTTOM : ' ');
  _lcd->write((s & SEG_D) ? BAR_D : ' ');
  _lcd->write((s & SEG_C) ? RIGHT_BOTTOM : ' ');
}
