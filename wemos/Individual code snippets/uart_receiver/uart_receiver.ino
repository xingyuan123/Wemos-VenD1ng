/*
 * written by xing yuan
 * 
 * UART receiver code
 * 
 * code is modified based on studying the examples from the sources below
 * https://linuxhint.com/serial-uart-communication-between-two-arduino/
 */
//Receiver Arduino Board Code
char code[2] = "0";               //Initialized variable to store receive
void setup() {
 Serial.begin(9600);             // Begin the Serial at 9600 Baud
 pinMode(D7,OUTPUT);
}
void loop() {
if(Serial.available()>0){
   Serial.readBytes(code, 1);  //Read the serial data
   int codeInt = String(code).toInt();
    switch(codeInt){
          case 2:

                digitalWrite(D7,HIGH);
                delay(1000);
                digitalWrite(D7,LOW);
                
            break; 

 }
}

}
