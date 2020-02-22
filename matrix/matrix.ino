// Arduino MINI ATmega168

// include the library code:
#include <LiquidCrystalFast.h>
#include <EEPROM.h>
#include "BigNumbers.h"
//#include "VerticalBarGraph.h"
#include "HBar.h"
#include "VBar.h"

// Bit Rate - using 19200 but LCD Smartie defaults to 9600
const int baud = 19200;

// EEPROM address values
const byte BRIGHTNESS = 0;	// Brightness
const byte CONTRAST = 1;	// Contrast
const byte SERIAL_LO = 2;	// Serial number low byte
const byte SERIAL_HI = 3;	// Serial number high byte

// IO Pins
const byte GPIO = 13;		// D13 - Built in LED on Nano V3.0
const byte backLight = 10;	// D10 - Use PWM to change brightness
const byte contrast = 3;    // D3  - Use PWM to change contrast  Connect 100uF between pin and GND

// Variables used in code - can probably optimise some of these out....
byte rxbyte;
byte temp;
byte addr;
byte level;
int val;
int i;

byte lcdRow;
byte lcdCol;

byte data[8];  // buffer for user character data

byte serial_getch();

LiquidCrystalFast lcd(7, 9, 8, 6,  3, 2, 5, 4);

HBar hBar = HBar(lcd);
VBar vBar = VBar(lcd);
BigNumbers bigNumbers = BigNumbers(lcd);

// Setup
void setup() {
  // Set the use ouf our output pins
  pinMode(GPIO, OUTPUT);
  //pinMode(backLight, OUTPUT);
  //pinMode(contrast, OUTPUT);

  // Gather default values from EEPROM (could add code here to handle default if EEPROM has never been written to)
  digitalWrite(GPIO, LOW);
  //analogWrite(backLight, EEPROM.read(BRIGHTNESS));    //  Read brightness level out of EEPROM
  //analogWrite(contrast, EEPROM.read(CONTRAST));       // Read contrast level out of EEPROM

  // set up the LCD's number of columns and rows:
  lcd.begin(40, 4);
  lcd.clear();

  lcd.print(" Matrix Orbital Display - version 1.0 ");
  lcd.setCursor(8, 2);

  lcd.print("Input Ready");
  lcd.setCursor(8, 3);
  lcd.print("19200,8,N,1");
  Serial.begin(baud);
  // Wait until we receive some data - note we want to hold what data it is as we use it the first iteration
  rxbyte = serial_getch();

  lcd.clear();
}

// Main loop - note we come into it with rxbyte already set from exit of Setup() function
void loop() {
  if (rxbyte == 254) //Matrix Orbital uses 254 prefix for commands
  {
    switch (serial_getch())
    {
      case 35: //Large digit
        temp = serial_getch();
        //bigNumbers.PrintBigCharOnPosition(serial_getch(), temp);
        bigNumbers.PrintBigChar(serial_getch(), temp);
        break;
      case 38: //pollKeyBuffer - send back key pressed
        Serial.write(0); //66 - up, 67 - right, 68 - left, 72 - down, 69 - center
        break;
      case 52: //set serial number /*USING EEPROM*/
        EEPROM.write(SERIAL_HI, serial_getch());
        EEPROM.write(SERIAL_LO, serial_getch());
        break;
      case 53: //read serial number /*USING EEPROM*/
        Serial.write(EEPROM.read(SERIAL_HI));
        Serial.write(EEPROM.read(SERIAL_LO));
        break;
      case 54: //read version number
        Serial.write(0x11);        //'v1.1
        break;
      case 55: //read module type
        Serial.write(0x07);        //'lcd_type'='LCD4041'
        break;
      case 59: //exit flow-control mode
        break;
      case 61: //Thick bargraph 5x8 char => 4 row x 8 pixels = 32 pixels
        temp = serial_getch();
        vBar.Draw(temp, serial_getch());
        break;
      case 64: // EEPROM Write (address, value)
        addr = serial_getch();
        val = serial_getch();
        EEPROM.write(addr, val);
        break;
      case 65: // EEPROM Read  (address)
        addr = serial_getch(); // EEPROM address
        val = EEPROM.read(addr); //
        Serial.print(val);
        break;
      case 66: //backlight on (at previously set brightness)
        // not implemented
        //analogWrite(backLight, level);
        temp = serial_getch();   // time value - not used
        break;
      case 67: //auto line-wrap on
      case 68: //auto line-wrap off
        break;
      case 70: //backlight off
        //analogWrite(backLight, 0);
        break;
      case 71:  //set cursor position
        lcdCol = (serial_getch() - 1);  //get column byte
        lcdRow = (serial_getch() - 1);  //get column byte
        lcd.setCursor(lcdCol, lcdRow);
        break;
      case 72:  //cursor home (reset display position)
        lcd.setCursor(0, 0);
        break;
      case 74:  //show underline cursor
        lcd.command(0b00001110);
        break;
      case 75:  //underline cursor off
      case 84:  //block cursor off
        lcd.command(0b00001100);
        break;
      case 76:  //move cursor left
        //MoveCursorLeft();
        //lcd.setCursor(lcdCol, lcdRow);
        lcd.command(16);
        break;
      case 77:  //move cursor right
        //MoveCursorRight();
        //lcd.setCursor(lcdCol, lcdRow);
        lcd.command(20);
        break;
      case 78:  //define custom char
        temp = serial_getch();  // Character ram value
        for (i = 0; i < 8; i++) {
          data[i] = serial_getch();
        }
        lcd.createChar(temp, data);
        break;
      case 79: //autoTxKeysOff
        break;
      case 80:  // Set contrast (but we save anyway)
      case 145: // Set Contrast and save
        level = 255 - serial_getch(); // Contrast value
        //analogWrite(contrast, level);
        EEPROM.write(CONTRAST, level);
        break;
      case 81: //auto scroll on
      case 82: //auto scroll off
        break;
      case 83:  //show blinking block cursor
        lcd.command(0b00001111);
        break;
      case 86:  //GPO OFF
        temp = serial_getch(); // GPIO Pin
        digitalWrite(GPIO, LOW);
        break;
      case 87:  //GPO ON
        temp = serial_getch(); // GPIO Pin
        digitalWrite(GPIO, HIGH);
        break;
      case 88:  //clear display, cursor home
        lcd.clear();
        break;
      case 96: //auto-repeat mode off (keypad)
        break;
      case 98: //Draw bitmap
        temp = serial_getch(); // refid
        temp = serial_getch(); // x
        temp = serial_getch(); // y
        break;
      case 99: // set drawing color
        temp = serial_getch();
        break;
      case 101: //Draw line continue
        temp = serial_getch(); // x
        temp = serial_getch(); // y
        break;
      case 104: //init horiz bar graph
        hBar.Init();
        break;
      case 108:
        temp = serial_getch(); // x1
        temp = serial_getch(); // y1
        temp = serial_getch(); // x2
        temp = serial_getch(); // y2
        break;
      case 109: //init med size digits
        break;
      case 110: //init lagre size digits
        lcd.clear();
        lcd.home();
        lcdCol = 0;
        lcdRow = 0;
        bigNumbers.Init();
        break;
      case 112: // Draw Pixel
        temp = serial_getch(); // x
        temp = serial_getch(); // y
        break;
      case 114: //draw rect
        temp = serial_getch(); // colour
        temp = serial_getch(); // x1
        temp = serial_getch(); // y1
        temp = serial_getch(); // x2
        temp = serial_getch(); // y2
        break;
      case 115: //init narrow vert bar graph
        vBar.Init(B00001100);
        break;
      case 118: //init wide vert bar graph
        vBar.Init(B00011111);
        break;
      case 120: //draw rect solid
        temp = serial_getch(); // colour
        temp = serial_getch(); // x1
        temp = serial_getch(); // y1
        temp = serial_getch(); // x2
        temp = serial_getch(); // y2
        break;
      case 124: //horizontal bar graph
        hBar.Col = serial_getch();
        hBar.Row = serial_getch();
        temp = serial_getch();
        hBar.Draw(temp, serial_getch());
        break;
      case 130: //setBacklightColour
        temp = serial_getch(); // red
        temp = serial_getch(); // green
        temp = serial_getch(); // blue
        break;
      // case 145:  //defined above see case 80:
      case 152: //set brightness and save
        level = serial_getch();
        //analogWrite(backLight, level);
        EEPROM.write(BRIGHTNESS, level);
        break;
      case 153: //set backlight brightness
        level = serial_getch();
        //analogWrite(backLight, level);
        break;
      case 160: //Report mode - Transmission protocol select
        temp = serial_getch(); // 0 - i2c, 1 - serial
        break;
      case 208: //setBacklightColour
        temp = serial_getch(); // red
        temp = serial_getch(); // green
        temp = serial_getch(); // blue
        break;
      case 209: // set the size of the display if it isn't 16x2 (you only have to do this once)
        temp = serial_getch(); //number of columns
        temp = serial_getch(); //number of rows
        break;
      default:
        //all other commands ignored and parameter byte discarded
        //temp = serial_getch();  //dump the command code
        break;
    }
  } //END OF COMMAND HANDLER
  else
  {
    // Not a command character but still might be a special character
    //change accented char to plain, detect and change descenders
    switch (rxbyte)
    {
      //chars that have direct equivalent in LCD charmap
      case 0xE4: //ASCII "a" umlaut
        rxbyte = 0xE1;
        break;
      case 0xF1: //ASCII "n" tilde
        rxbyte = 0xEE;
        break;
      case 0xF6: //ASCII "o" umlaut
        rxbyte = 0xEF; //was wrong in v0.86
        break;
      case 0xFC: //ASCII "u" umlaut
        rxbyte = 0xF5;
        break;
      //accented -> plain equivalent
      //and misc symbol translation
      case 0xA3: //sterling (pounds)
        rxbyte = 0xED;
        break;
      case 0xAC: //sterling (pounds)
        rxbyte = 0xB0;
        break;
      case 0xB0: //degrees symbol
        rxbyte = 0xDF;
        break;
      case 0xB5: //mu
        rxbyte = 0xE4;
        break;
      case 0xC0: //"A" variants
      case 0xC1:
      case 0xC2:
      case 0xC3:
      case 0xC4:
      case 0xC5:
        rxbyte = 0x41;
        break;
      case 0xC8: //"E" variants
      case 0xC9:
      case 0xCA:
      case 0xCB:
        rxbyte = 0x45;
        break;
      case 0xCC: //"I" variants
      case 0xCD:
      case 0xCE:
      case 0xCF:
        rxbyte = 0x49;
        break;
      case 0xD1: //"N" tilde -> plain "N"
        rxbyte = 0x43;
        break;
      case 0xD2: //"O" variants
      case 0xD3:
      case 0xD4:
      case 0xD5:
      case 0xD6:
      case 0xD8:
        rxbyte = 0x4F;
        break;
      case 0xD9: //"U" variants
      case 0xDA:
      case 0xDB:
      case 0xDC:
        rxbyte = 0x55;
        break;
      case 0xDD: //"Y" acute -> "Y"
        rxbyte = 0x59;
        break;
      case 0xE0: //"a" variants except umlaut
      case 0xE1:
      case 0xE2:
      case 0xE3:
      case 0xE5:
        rxbyte = 0x61;
        break;
      case 0xE7: //"c" cedilla -> "c"
        rxbyte = 0x63;
        break;
      case 0xE8: //"e" variants
      case 0xE9:
      case 0xEA:
      case 0xEB:
        rxbyte = 0x65;
        break;
      case 0xEC: //"i" variants
      case 0xED:
      case 0xEE:
      case 0xEF:
        rxbyte = 0x69;
        break;
      case 0xDF: //beta  // LCDSmartie degree symbol??
      case 0xF2: //"o" variants except umlaut
      case 0xF3:
      case 0xF4:
      case 0xF5:
      case 0xF8:
        rxbyte = 0x6F;
        break;
      case 0xF7: //division symbol
        rxbyte = 0xFD;
        break;
      case 0xF9: //"u" variants except umlaut
      case 0xFA:
      case 0xFB:
        rxbyte = 0x75;
        break;
      default:
        break;
    }

    switch (rxbyte)
    {
      case 0x08:
        lcd.command(16);
        lcd.write(32);
        lcd.command(16);
        /*
                MoveCursorLeft();
                lcd.setCursor(lcdCol, lcdRow);
                lcd.write(32);
                MoveCursorLeft();
                lcd.setCursor(lcdCol, lcdRow);
        */
        break;
      /*
            case 0x0A: //0x0A - Moves cursor to the beginning of the next (or previous) line
              lcdCol = 0;
              MoveNextRow();
              lcd.setCursor(lcdCol, lcdRow);
              break;
            case 0x0D: //0x0D - Moves cursor to the beginning of the current line
              lcdCol = 0;
              lcd.setCursor(lcdCol, lcdRow);
              break;
      */
      case 0x0C:
        lcd.clear();
        lcd.home();
        lcdCol = 0;
        lcdRow = 0;
        break;
      default:
        lcd.write(rxbyte);  //print it to lcd
        break;
    }
  }
  rxbyte = serial_getch();    // Wait for the next byte and use value for next iteration
}

// Helper function to read serial input as clean 8 bit byte
byte serial_getch() {
  int ch;
  while (Serial.available() == 0) {}
  // read the incoming byte:
  ch = Serial.read();
  return (byte)(ch & 0xff);
}
