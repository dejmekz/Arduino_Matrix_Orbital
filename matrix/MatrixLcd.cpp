#include "MatrixLcd.h"

size_t MatrixLcd::write(uint8_t value)
{
  size_t n = LiquidCrystalFast::write(value);

  // At the end of a row the library defers the move to the next row until
  // the next character. Do it now so the visible cursor is in the right place.
  if (_setCursFlag)
    setCursor(_x, _y);

  syncCursor();
  return n;
}

void MatrixLcd::moveTo(uint8_t col, uint8_t row)
{
  if (col >= _numcols)
    col = _numcols - 1;

  if (row >= _numlines)
    row = _numlines - 1;

  setCursor(col, row);
  syncCursor();
}

void MatrixLcd::clearScreen()
{
  clear();
  syncCursor();
}

void MatrixLcd::cursorLeft()
{
  if (_x > 0)
    moveTo(_x - 1, _y);
  else if (_y > 0)
    moveTo(_numcols - 1, _y - 1);
}

void MatrixLcd::cursorRight()
{
  if (_x < _numcols - 1)
    moveTo(_x + 1, _y);
  else if (_y < _numlines - 1)
    moveTo(0, _y + 1);
}

void MatrixLcd::backspace()
{
  cursorLeft();
  write(' ');
  cursorLeft();
}

void MatrixLcd::carriageReturn()
{
  moveTo(0, _y);
}

void MatrixLcd::lineFeed()
{
  moveTo(0, _y < _numlines - 1 ? _y + 1 : 0);
}

void MatrixLcd::setCursorMode(uint8_t mode)
{
  _cursorMode = mode & (LCD_CURSORON | LCD_BLINKON);

  // hide the cursor on both controllers, then show it on the active one
  commandBoth(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
  _cursorChip = _chip;
  showCursor();
}

// LiquidCrystalFast only turns the underline off on the controller it leaves
// and never turns the cursor on for the new one, so move it ourselves.
void MatrixLcd::syncCursor()
{
  if (_chip == _cursorChip)
    return;

  uint8_t chip = _chip;
  _chip = _cursorChip;
  send(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LOW);
  _chip = chip;
  _cursorChip = chip;
  showCursor();
}

// command() is inline in LiquidCrystalFast.cpp, so use send() from here
void MatrixLcd::showCursor()
{
  _displaycontrol = LCD_DISPLAYON | _cursorMode;
  send(LCD_DISPLAYCONTROL | _displaycontrol, LOW);
}
