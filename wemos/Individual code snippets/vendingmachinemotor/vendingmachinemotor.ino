/*
 * written by xing yuan 
 * This code snippet uses a button to dispense a snack 
 * 
 * code is modified based on studying the examples from the source below 
 * https://www.instructables.com/How-to-use-the-L293D-Motor-Driver-Arduino-Tutorial/
 */
#define motorPin1 D5  // Pin 14 of L293
#define motorPin2 D6  // Pin 10 of L293


void setup(){
    //Set pins as outputs
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    Serial.begin(9600);

    //set button as input
    pinMode(D4,INPUT_PULLUP);
    
    
    //This code will turn Motor A counter-clockwise for 2 sec.
    //digitalWrite(motorPin1, LOW);
    //digitalWrite(motorPin2, HIGH);
    //delay(2000);
    
   
}
void loop(){
  if(digitalRead(D4) == HIGH){
    Serial.println("button not Pressed");
    
  } else {
    Serial.println("button Pressed");
    //Motor Control - Motor A: motorPin1,motorpin2 
    //This code  will turn Motor A clockwise for 2 sec.
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    delay(500); 
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);

  }
  delay(1000);
}
