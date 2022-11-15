/*
 * written by xing yuan 
 * 
 * UART sender test code 
 * 
 *  code is modified based on studying the examples from the sources below
 *  https://linuxhint.com/serial-uart-communication-between-two-arduino/
 */

volatile unsigned long lastStartTime = 0;
volatile bool startTriggered = false;
int debounceDelay = 50;

void setup() {
  Serial.begin(9600);
  pinMode(D4,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(D4),pressed ,FALLING); 

}
IRAM_ATTR void pressed() {
  int currTime = millis();
  if(currTime - lastStartTime > debounceDelay){
    startTriggered = true; 
    lastStartTime = currTime;
  }
}


void loop() {
  if(startTriggered == true){
    
    Serial.write("2");
    startTriggered = false;
  }
}
