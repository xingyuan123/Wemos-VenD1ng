/*
 * Written by xing yuan 
 * 
 * final code for slave2 of vending machine
 * control i2c LCD
 * 
 */

#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

//UART
char code[4] = "0";   
int codeInt = 000;

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop(){
if(Serial.available()>0){
        Serial.readBytes(code, 3);  //Read the serial data
        Serial.println(code);
        codeInt = String(code).toInt();   
        switch(codeInt){
          case 101:
            lcd.backlight();
            lcd.setCursor(0, 0); 
            lcd.print("Selected Snack 1");            
            codeInt = 0;
            break;
          case 102:
            lcd.backlight();
            lcd.setCursor(0, 0); 
            lcd.print("Selected Snack 2");
            break;
          case 105:
            lcd.backlight();
            lcd.setCursor(0, 0); 
            lcd.print("Selected Jackpot");
            codeInt = 0;      
            break;
          case 106:
            lcd.clear();  
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Please pay 20c");            
            codeInt = 0;                
            break;
          case 107:
            lcd.clear();  
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Please pay 30c");
            codeInt = 0;
            break;        
          case 108:
            lcd.setCursor(0, 0);
            lcd.print("Inserted: 10c");
            codeInt = 0;
            break;
          case 109:
            lcd.setCursor(0, 0);
            lcd.print("Inserted: 20c");
            codeInt = 0;
            break;
          case 110:
            lcd.setCursor(0, 0);
            lcd.print("Invalid coin");
            codeInt = 0;            
            break;
          case 111:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Cancelled");           
            codeInt = 0;            
            break;
          case 112:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Jackpot failed :(");
            codeInt = 0;
            break;
          case 113:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("JACKPOT!!");
            codeInt = 0;
            break;
          case 114:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Snack dispensed");
            codeInt = 0;
            break;
          case 115:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Thank you please");
            lcd.setCursor(0, 1);
            lcd.print("come again :)");
            break
          case 116:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Tilted!");
            break;
          case 117:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Remaining: 10c");
            break;
          case 118:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Remaining: 20c");
            break;
          case 119:
            lcd.clear();             
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Remaining: 30c");
            break;         
          default:
            //do nothing 
            break;
      }
    }       
}
