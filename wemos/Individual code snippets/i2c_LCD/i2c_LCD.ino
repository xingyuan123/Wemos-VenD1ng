#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

/* Written by xing yuan
 * 
 * this code snippet uses I2C LCD to display output of a system 
 * need to install LiquidCrystal I2C library by Frank de Brabander befor using  
 * https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
 * 
 * coded is modified based on studying the example from the 2 source below
 * https://create.arduino.cc/projecthub/Arnov_Sharma_makes/lcd-i2c-tutorial-664e5a
 */

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop(){
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("hello");
  delay(1000);
  // clears the display to print new message
  lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("world");
  delay(1000);
  lcd.clear(); 
}
