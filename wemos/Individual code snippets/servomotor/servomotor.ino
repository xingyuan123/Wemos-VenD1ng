/*
 * written by xing yuan
 * 
 * this code test for servo motor rotation
 * Servo library is needed to run this cod
 * https://www.arduino.cc/reference/en/libraries/servo/
 * 
 * code is modified based on studying the examples from the sources below
 * https://docs.arduino.cc/learn/electronics/servo-motors
 */


#include <Servo.h>
//servo motor
// Brown = GND
//RED = 5V
//Orange = Signal


Servo myservo1;  //create servo motor 1 object
Servo myservo2;  //create servo motor 2 object  
Servo myservo3;  //create servo motor 3 object   


int pos = 0;    // variable to store the servo position

#define servomotor1 D6 // servo 1
#define servomotor2 D7 // servo 2
#define servomotor3 D8 // servo 3

void setup() {
  Serial.begin(9600);
  
  myservo1.attach(servomotor1);
  myservo2.attach(servomotor2);
  myservo3.attach(servomotor3);
  pinMode(D4,INPUT_PULLUP);
  myservo1.write(0);  //servo1
  myservo2.write(90); //servo2
  myservo3.write(90); //servo3 


}

  void loop() {  
    if(digitalRead(D4) == HIGH){
    Serial.println("button not Pressed");
    
  } else {
        Serial.println("button Pressed");
        myservo1.write(180);
        delay(1000);
        myservo1.write(0);

        myservo2.write(0);// or 0
        delay(1000);
        myservo2.write(90);
        delay(1000);
        myservo2.write(180);// or 180 
        delay(1000);
        myservo2.write(90);        
  }
}

//servo3 
/*
  myservo3.write(180);// or 0
  delay(1000);
  myservo3.write(90);
  delay(1000);
  myservo3.write(0);// or 180 
  delay(1000);
  myservo3.write(90);      
 */
   
