// Matrix Orbital LCD4041 emulation on a 40x4 HD44780 display
// Arduino Nano V3.0 (ATmega328P) or Arduino Mini (ATmega168)

#include <EEPROM.h>
#include "MatrixLcd.h"
#include "BigNumbers.h"
#include "HBar.h"
#include "VBar.h"

// Bit Rate - using 19200 but LCD Smartie defaults to 9600
const long baud = 19200;

// EEPROM address values
const byte BRIGHTNESS = 0;	// Brightness
                        	// 1 - was contrast, contrast is set by a trimmer
const byte SERIAL_LO = 2;	// Serial number low byte
const byte SERIAL_HI = 3;	// Serial number high byte
const byte EEPROM_MAGIC = 4;	// EEPROM_MAGIC_VALUE once the defaults are written
const byte STARTUP_SET = 5;	// 1 - custom startup screen stored, 0 - built-in one
const int STARTUP_SCREEN = 16;	// custom startup screen, LCD_COLS * LCD_ROWS bytes

const byte EEPROM_MAGIC_VALUE = 0xA5;

// IO Pins
const byte GPIO = 13;		// D13 - Built in LED on Nano V3.0
const byte backLight = 10;	// D10 - Use PWM to change brightness

// Display size
const byte LCD_COLS = 40;
const byte LCD_ROWS = 4;

// Matrix Orbital uses 0xFE prefix for commands
const byte COMMAND_PREFIX = 0xFE;

// Matrix Orbital commands that are implemented (sent after COMMAND_PREFIX)
enum MatrixCommand : byte {
  CMD_LARGE_DIGIT       = 0x23,  // column, digit
  CMD_POLL_KEYPAD       = 0x26,
  CMD_SET_SERIAL        = 0x34,  // hi, lo
  CMD_READ_SERIAL       = 0x35,
  CMD_READ_VERSION      = 0x36,
  CMD_READ_MODULE_TYPE  = 0x37,
  CMD_VBAR              = 0x3D,  // column, height
  CMD_STARTUP_SCREEN    = 0x40,  // one char per display cell
  CMD_BACKLIGHT_ON      = 0x42,  // minutes
  CMD_BACKLIGHT_OFF     = 0x46,
  CMD_GOTO              = 0x47,  // column, row
  CMD_HOME              = 0x48,
  CMD_UNDERLINE_ON      = 0x4A,
  CMD_UNDERLINE_OFF     = 0x4B,
  CMD_CURSOR_LEFT       = 0x4C,
  CMD_CURSOR_RIGHT      = 0x4D,
  CMD_CUSTOM_CHAR       = 0x4E,  // slot, 8 bytes of bitmap
  CMD_BLOCK_ON          = 0x53,
  CMD_BLOCK_OFF         = 0x54,
  CMD_GPO_OFF           = 0x56,  // gpo number
  CMD_GPO_ON            = 0x57,  // gpo number
  CMD_CLEAR             = 0x58,
  CMD_INIT_HBAR         = 0x68,
  CMD_INIT_LARGE_DIGITS = 0x6E,
  CMD_INIT_VBAR_NARROW  = 0x73,
  CMD_INIT_VBAR_WIDE    = 0x76,
  CMD_HBAR              = 0x7C,  // column, row, direction, length
  CMD_BRIGHTNESS_SAVE   = 0x98,  // level
  CMD_BRIGHTNESS        = 0x99,  // level
};

// Commands that are accepted but ignored, with the number of parameter bytes
// to discard. Commands not listed here and not implemented have no parameters
// (wrap, scroll, key and flow-control switches, medium digit init, ...).
const byte IGNORED_COMMANDS[][2] PROGMEM = {
  {0x3A, 2},  // enter flow-control mode: full, empty
  {0x50, 1},  // set contrast - contrast is set by a trimmer
  {0x62, 3},  // draw bitmap: refid, x, y
  {0x63, 1},  // set drawing color
  {0x65, 2},  // continue line: x, y
  {0x6C, 4},  // draw line: x1, y1, x2, y2
  {0x6F, 3},  // place medium digit: row, column, digit
  {0x70, 2},  // draw pixel: x, y
  {0x72, 5},  // draw rectangle: color, x1, y1, x2, y2
  {0x78, 5},  // draw solid rectangle: color, x1, y1, x2, y2
  {0x82, 3},  // set backlight colour: red, green, blue
  {0x91, 1},  // set and save contrast - contrast is set by a trimmer
  {0xA0, 1},  // transmission protocol select: 0 - i2c, 1 - serial
  {0xD0, 3},  // set backlight colour: red, green, blue
  {0xD1, 2},  // set display size: columns, rows
};

// Host characters 0x80 - 0xFF (ISO 8859-1) mapped to the HD44780 A00 ROM:
// direct equivalents where the ROM has them, otherwise the plain letter
const byte CHAR_MAP[128] PROGMEM = {
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,  // 0x80
  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,  // 0x88
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,  // 0x90
  0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,  // 0x98
  0xA0, 0xA1, 0xA2, 0xED, 0xA4, 0xA5, 0xA6, 0xA7,  // 0xA0  pound
  0xA8, 0xA9, 0xAA, 0xAB, 0xB0, 0xAD, 0xAE, 0xAF,  // 0xA8  not sign
  0xDF, 0xB1, 0xB2, 0xB3, 0xB4, 0xE4, 0xB6, 0xB7,  // 0xB0  degree, mu
  0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,  // 0xB8
  0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7,  // 0xC0  A variants
  0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,  // 0xC8  E, I variants
  0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7,  // 0xD0  N tilde, O variants
  0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0xDE, 0x6F,  // 0xD8  U variants, Y acute, sharp s (LCD Smartie degree?)
  0x61, 0x61, 0x61, 0x61, 0xE1, 0x61, 0xE6, 0x63,  // 0xE0  a variants, a umlaut, c cedilla
  0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,  // 0xE8  e, i variants
  0xF0, 0xEE, 0x6F, 0x6F, 0x6F, 0x6F, 0xEF, 0xFD,  // 0xF0  n tilde, o variants, o umlaut, division
  0x6F, 0x75, 0x75, 0x75, 0xF5, 0xFD, 0xFE, 0xFF,  // 0xF8  u variants, u umlaut
};

byte brightness;    // backlight level, restored from EEPROM

MatrixLcd lcd(7, 9, 8, 6,  3, 2, 5, 4);

HBar hBar = HBar(lcd);
VBar vBar = VBar(lcd);
BigNumbers bigNumbers = BigNumbers(lcd);

bool firstByte = true;  // splash screen is shown until the first byte arrives

byte serial_getch();
void serial_skip(int count);
byte clamp1(byte value, byte limit);
void handleCommand(byte command);
void handleChar(byte value);
byte translateChar(byte value);
void loadSettings();
void showStartupScreen();
void saveStartupScreen();

// Setup
void setup() {
  // Set the use ouf our output pins
  pinMode(GPIO, OUTPUT);
  //pinMode(backLight, OUTPUT);

  digitalWrite(GPIO, LOW);
  loadSettings();
  //analogWrite(backLight, brightness);

  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  showStartupScreen();

  Serial.begin(baud);
}

// Restore settings from EEPROM, writing defaults on a blank chip
void loadSettings() {
  if (EEPROM.read(EEPROM_MAGIC) != EEPROM_MAGIC_VALUE) {
    EEPROM.update(BRIGHTNESS, 255);
    EEPROM.update(SERIAL_LO, 0);
    EEPROM.update(SERIAL_HI, 0);
    EEPROM.update(STARTUP_SET, 0);
    EEPROM.update(EEPROM_MAGIC, EEPROM_MAGIC_VALUE);
  }

  brightness = EEPROM.read(BRIGHTNESS);
}

void showStartupScreen() {
  lcd.clearScreen();

  if (EEPROM.read(STARTUP_SET) == 1) {
    for (byte row = 0; row < LCD_ROWS; row++) {
      lcd.moveTo(0, row);
      for (byte col = 0; col < LCD_COLS; col++) {
        lcd.write(translateChar(EEPROM.read(STARTUP_SCREEN + row * LCD_COLS + col)));
      }
    }
    return;
  }

  lcd.print(F(" Matrix Orbital Display - version 1.0 "));
  lcd.setCursor(8, 2);
  lcd.print(F("Input Ready"));
  lcd.setCursor(8, 3);
  lcd.print(baud);
  lcd.print(F(",8,N,1"));
}

// Store the startup screen sent with 0x40, all spaces restore the built-in one.
// The whole screen is received before writing, as EEPROM writes (3.3 ms per
// byte) are slower than the serial input and would overflow its buffer.
void saveStartupScreen() {
  byte screen[LCD_COLS * LCD_ROWS];
  bool blank = true;

  for (int i = 0; i < LCD_COLS * LCD_ROWS; i++) {
    screen[i] = serial_getch();
    if (screen[i] != ' ')
      blank = false;
  }

  if (blank) {
    EEPROM.update(STARTUP_SET, 0);
    return;
  }

  for (int i = 0; i < LCD_COLS * LCD_ROWS; i++) {
    EEPROM.update(STARTUP_SCREEN + i, screen[i]);
  }
  EEPROM.update(STARTUP_SET, 1);
}

void loop() {
  byte rxbyte = serial_getch();

  if (firstByte) {
    lcd.clearScreen();
    firstByte = false;
  }

  if (rxbyte == COMMAND_PREFIX)
    handleCommand(serial_getch());
  else
    handleChar(rxbyte);
}

void handleCommand(byte command) {
  byte col, row, value;
  byte data[8];  // buffer for user character data

  switch (command)
  {
    case CMD_LARGE_DIGIT:
      col = serial_getch();
      value = serial_getch();
      //bigNumbers.PrintBigCharOnPosition(value, col);
      if (col <= LCD_COLS - 3) {  // digit is 3 columns wide
        bigNumbers.PrintBigChar(value, col);
      }
      break;
    case CMD_POLL_KEYPAD:  //send back key pressed
      Serial.write(0); //66 - up, 67 - right, 68 - left, 72 - down, 69 - center
      break;
    case CMD_SET_SERIAL:
      EEPROM.update(SERIAL_HI, serial_getch());
      EEPROM.update(SERIAL_LO, serial_getch());
      break;
    case CMD_READ_SERIAL:
      Serial.write(EEPROM.read(SERIAL_HI));
      Serial.write(EEPROM.read(SERIAL_LO));
      break;
    case CMD_READ_VERSION:
      Serial.write(0x11);        //'v1.1
      break;
    case CMD_READ_MODULE_TYPE:
      Serial.write(0x07);        //'lcd_type'='LCD4041'
      break;
    case CMD_VBAR: //Thick bargraph 5x8 char => 4 row x 8 pixels = 32 pixels
      col = serial_getch();  //column, 1-based
      vBar.Draw(clamp1(col, LCD_COLS), serial_getch());
      break;
    case CMD_STARTUP_SCREEN: //shown on the next power up
      saveStartupScreen();
      break;
    case CMD_BACKLIGHT_ON: //at previously set brightness
      serial_getch();   // time value - not used
      //analogWrite(backLight, brightness);
      break;
    case CMD_BACKLIGHT_OFF:
      //analogWrite(backLight, 0);
      break;
    case CMD_GOTO:
      col = serial_getch();  //column, 1-based
      row = serial_getch();  //row, 1-based
      lcd.moveTo(clamp1(col, LCD_COLS), clamp1(row, LCD_ROWS));
      break;
    case CMD_HOME:
      lcd.moveTo(0, 0);
      break;
    case CMD_UNDERLINE_ON:
      lcd.setCursorMode(lcd.cursorMode() | LCD_CURSORON);
      break;
    case CMD_UNDERLINE_OFF:
      lcd.setCursorMode(lcd.cursorMode() & ~LCD_CURSORON);
      break;
    case CMD_CURSOR_LEFT:
      lcd.cursorLeft();
      break;
    case CMD_CURSOR_RIGHT:
      lcd.cursorRight();
      break;
    case CMD_CUSTOM_CHAR:
      value = serial_getch();  // Character ram value
      for (byte i = 0; i < 8; i++) {
        data[i] = serial_getch();
      }
      lcd.createChar(value, data);
      break;
    case CMD_BLOCK_ON:
      lcd.setCursorMode(lcd.cursorMode() | LCD_BLINKON);
      break;
    case CMD_BLOCK_OFF:
      lcd.setCursorMode(lcd.cursorMode() & ~LCD_BLINKON);
      break;
    case CMD_GPO_OFF:
      serial_getch(); // GPO number - only one GPO
      digitalWrite(GPIO, LOW);
      break;
    case CMD_GPO_ON:
      serial_getch(); // GPO number - only one GPO
      digitalWrite(GPIO, HIGH);
      break;
    case CMD_CLEAR: //clear display, cursor home
      lcd.clearScreen();
      break;
    case CMD_INIT_HBAR:
      hBar.Init();
      break;
    case CMD_INIT_LARGE_DIGITS:
      lcd.clearScreen();
      bigNumbers.Init();
      break;
    case CMD_INIT_VBAR_NARROW:
      vBar.Init(B00001100);
      break;
    case CMD_INIT_VBAR_WIDE:
      vBar.Init(B00011111);
      break;
    case CMD_HBAR:
      hBar.Col = serial_getch();
      hBar.Row = serial_getch();
      value = serial_getch();  // direction
      hBar.Draw(value, serial_getch());
      break;
    case CMD_BRIGHTNESS_SAVE:
      brightness = serial_getch();
      //analogWrite(backLight, brightness);
      EEPROM.update(BRIGHTNESS, brightness);
      break;
    case CMD_BRIGHTNESS:
      brightness = serial_getch();
      //analogWrite(backLight, brightness);
      break;
    default:
      //all other commands ignored and their parameter bytes discarded
      for (byte i = 0; i < sizeof(IGNORED_COMMANDS) / sizeof(IGNORED_COMMANDS[0]); i++) {
        if (pgm_read_byte(&IGNORED_COMMANDS[i][0]) == command) {
          serial_skip(pgm_read_byte(&IGNORED_COMMANDS[i][1]));
          break;
        }
      }
      break;
  }
}

//change accented char to plain or to its LCD charmap equivalent
byte translateChar(byte value) {
  if (value >= 0x80)
    return pgm_read_byte(&CHAR_MAP[value - 0x80]);
  return value;
}

void handleChar(byte value) {
  value = translateChar(value);

  switch (value)
  {
    case 0x08: //backspace
      lcd.backspace();
      break;
    case 0x0A: //line feed - beginning of the next line
      lcd.lineFeed();
      break;
    case 0x0D: //carriage return - beginning of the current line
      lcd.carriageReturn();
      break;
    case 0x0C: //form feed - clear display
      lcd.clearScreen();
      break;
    default:
      lcd.write(value);  //print it to lcd
      break;
  }
}

// Helper function to read serial input as clean 8 bit byte
byte serial_getch() {
  int ch;
  while (Serial.available() == 0) {}
  // read the incoming byte:
  ch = Serial.read();
  return (byte)(ch & 0xff);
}

// Read and discard parameter bytes of a command we do not implement
void serial_skip(int count) {
  while (count-- > 0) {
    serial_getch();
  }
}

// Convert a 1-based Matrix Orbital coordinate to a 0-based one inside 0..limit-1
byte clamp1(byte value, byte limit) {
  if (value < 1) return 0;
  if (value > limit) return limit - 1;
  return value - 1;
}
