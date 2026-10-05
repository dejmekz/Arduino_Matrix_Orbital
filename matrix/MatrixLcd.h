#ifndef MatrixLcd_h
#define MatrixLcd_h

#include <Arduino.h>
#include <LiquidCrystalFast.h>

// LiquidCrystalFast with cursor movement that keeps its position tracking
// in sync. A 40x4 display has two HD44780 controllers (rows 0-1 and 2-3),
// so raw cursor commands would only reach one of them.
class MatrixLcd : public LiquidCrystalFast
{
  public:
    using LiquidCrystalFast::LiquidCrystalFast;
    using LiquidCrystalFast::write;

    virtual size_t write(uint8_t value);

    void moveTo(uint8_t col, uint8_t row);  // clamped to the display size
    void clearScreen();
    void cursorLeft();     // wraps to the end of the previous row
    void cursorRight();    // wraps to the start of the next row
    void backspace();
    void carriageReturn(); // start of the current row
    void lineFeed();       // start of the next row
    void setCursorMode(uint8_t mode);  // LCD_CURSORON | LCD_BLINKON, 0 = hidden
    uint8_t cursorMode() const { return _cursorMode; }

  private:
    uint8_t _cursorMode = 0;
    uint8_t _cursorChip = 0;  // controller that currently shows the cursor

    void syncCursor();
    void showCursor();
};

#endif
