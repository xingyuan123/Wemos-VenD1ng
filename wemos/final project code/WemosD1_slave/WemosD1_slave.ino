/*
 * Written by xing yuan 
 * 
 * final code for slave2 of vending machine
 * controls 3 servo, buzzer , 2 motor
 * 
 */


#include <Servo.h>

//sleep mode
#define FPM_SLEEP_MAX_TIME           0xFFFFFFF
#define LIGHT_WAKE_PIN 0 //corresponds to WeMos D3 pin

//servo
Servo myservo1;  //create servo motor 1 object
Servo myservo2;  //create servo motor 2 object  
Servo myservo3;  //create servo motor 3 object  

//servo
#define servomotor1 D4 // servo 1
#define servomotor2 D7 // servo 2
#define servomotor3 D8 // servo 3

//motor
//tam tam
#define motorPin1 D5  // Pin 15 of L293
#define motorPin2 D6  // Pin 10 of L293
//mi mi
#define motorPin3 D1  // Pin 7 of L293
#define motorPin4 D2  // Pin 2 of L293



//buzzer
#define buzzerPin D0

//buzzer
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784

//buzzer
int melody[] = {
  NOTE_G5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_G4,NOTE_A4,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,
  NOTE_E5,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_E4,NOTE_F4,NOTE_G4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_G4,NOTE_E4,NOTE_F4,NOTE_G4,
  NOTE_F4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_E4,NOTE_D4,NOTE_E4,NOTE_D4,NOTE_C4,NOTE_D4,NOTE_E4,NOTE_F4,NOTE_G4,NOTE_A4,
  NOTE_F4,NOTE_A4,NOTE_G4,NOTE_A4,NOTE_B4,NOTE_C5,NOTE_G4,NOTE_A4,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,NOTE_G5,
  NOTE_E5,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_D5,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_B4,
  NOTE_C5,NOTE_A4,NOTE_B4,NOTE_C5,NOTE_C4,NOTE_D4,NOTE_E4,NOTE_F4,NOTE_E4,NOTE_D4,NOTE_E4,NOTE_C5,NOTE_B4,NOTE_C5,
  NOTE_A4,NOTE_C5,NOTE_B4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_G4,NOTE_F4,NOTE_E4,NOTE_F4,NOTE_G4,NOTE_A4,NOTE_B4,NOTE_C4,
  NOTE_A4,NOTE_C5,NOTE_B4,NOTE_C5,NOTE_B4,NOTE_A4,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_C5,NOTE_B4,NOTE_C5,NOTE_A4,NOTE_B4
};

//buzzer
int noteDurations[] = {
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  8,16,16,8,16,16,16,16,16,16,16,16,16,16,
  
};

//buzzer
//play jackpot sound
void play(){
    for (int thisNote = 0; thisNote < 112; thisNote++) {

      // to calculate the note duration, take one second 
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000/noteDurations[thisNote];
      tone(buzzerPin,melody[thisNote],noteDuration);
      
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(buzzerPin);
  }
}

//UART
char code[4] = "0";   
int codeInt = 000;

//sleep mode
// Required for LIGHT_SLEEP_T delay mode
extern "C" {
#include "user_interface.h"
}

 
//The setup function is called once at startup of the sketch
void setup() {
  Serial.begin(9600);

  
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
  myservo1.attach(servomotor1);
  myservo2.attach(servomotor2);
  myservo3.attach(servomotor3);
  myservo1.write(0);  //servo1
  myservo2.write(90); //servo2
  myservo3.write(90); //servo3 
 
//  pinMode(LIGHT_WAKE_PIN,INPUT); //wake up GPIO
}

void dispenseSnack(int snackNumber){
  //see tam tam and mi mi
  if (snackNumber == 1){  //snack 1 is tamtam 
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    delay(400); 
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
  } else if (snackNumber == 2){//snack 2 is mimi
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(400); 
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
  }
  //servo 3 CW
  myservo3.write(180); 
  delay(1000);
  myservo3.write(90);
}

void coinAccept(){
  //servo 1 
  myservo1.write(180);
  delay(500);//need calibration
  myservo1.write(0);
  //servo 2 turn cw 
  myservo2.write(0);
  delay(1000); 
  myservo2.write(90);
}

void coinReject(){
  //servo 1 
  myservo1.write(180);
  delay(500);//need calibration
  myservo1.write(0);
  //servo 2 ACW
  myservo2.write(180);
  delay(1000); 
  myservo2.write(90);
  
}

//alarm
void buzzer(){
  tone(buzzerPin, 1000); // Send 1KHz sound signal...
  delay(1000*30); //30 seconds
  noTone(buzzerPin);     // Stop sound...
}

void coinReturn(){
  //servo 3 left 
  myservo3.write(180); //servo3
  delay(1000);
  myservo3.write(90); //back to original position
}

void loop() {
//      gpio_pin_wakeup_enable(GPIO_ID_PIN(LIGHT_WAKE_PIN), GPIO_PIN_INTR_LOLEVEL); //change to high level
//      wifi_set_opmode(NULL_MODE);
//      wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
//      wifi_fpm_open();
//      wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
//      delay(1000);
//      Serial.println("woke up");
      if(Serial.available()>0){
        Serial.readBytes(code, 3);  //Read the serial data
        Serial.println(code);
        codeInt = String(code).toInt();   
        switch(codeInt){
          case 201:
            //dispense snack 1
            Serial.println("tam tam");
            dispenseSnack(1);
            codeInt = 0;
            break;
          case 202:
            //dispense snack 2
            Serial.println("mi mi");
            dispenseSnack(2);  
            codeInt = 0;      
            break;
//          case 3:
//            //dispense snack 3
//            dispenseSnack(3);        
//            break;
//          case 4:
//            //dispense snack 4
//            dispenseSnack(4);        
//            break;
          case 205:
            //dispense all
            //jackpot win
            Serial.println("jackpot win");
            play(); //play win song
            dispenseSnack(1);        
            dispenseSnack(2);        
            dispenseSnack(3);        
            dispenseSnack(4);    
            codeInt = 0;                
            break;
          case 206:
            // accept coins 1 and 2
            Serial.println("accept");
            coinAccept();
            codeInt = 0;
            break;        
          case 207:
            // reject coins 1 and 2
            Serial.println("reject");
            coinReject();
            codeInt = 0;
            break;
          case 208:
            // sound off buzzer 
            Serial.println("buzzer");
            buzzer();
            codeInt = 0;
            break;
          case 209:
            //return coins
            Serial.println("return");
            coinReturn();
            codeInt = 0;            
            break;
          default:
            //do nothing 
            break;
      }
    }      
}
