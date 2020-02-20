#include "BigNumbers.h"

BigNumbers::BigNumbers(LiquidCrystalFast &lcd)
{
  _lcd = &lcd;
}

void BigNumbers::Init()
{
  for (int i = 0; i < 7; i++)
  {
    _lcd->createChar(i + 1, bigNumberChars[i]);
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
  for (int y = 0; y < 4; y++)
  {
    // print chars in columns
    for (int x = 0; x < 3; x++)
    {
      // 3 custom chars
      _lcd->setCursor(column + x, y);
      _lcd->write(bigNumbersArray[digit][y][x]);
    }
  }
}
