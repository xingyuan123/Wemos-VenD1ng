/*
 * Written by chuan an and xing yuan
 * 
 * wemos and server written by mayank
 * 
 * final code for master of vending machine
 */

#include <Wire.h> // for I2C communication
//#include <LiquidCrystal_I2C.h> 
#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>
#include <include/WiFiState.h>

// wifi details
const char* ssid = "OPPO A92";
const char* password = "cs3237boyd";
,
// MQTT details
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
const char broker[] = "34.143.248.115";
int        port     = 1883;
const char pub_topic[]  = "WEMOS/detectcoin"; 
const char pub_stolen[]  = "WEMOS/cs3237fml";
const char sub_dispense[] = "WEMOS/dispense";
const char sub_topic[] = "WEMOS/coindone";

//B for button
#define knockSensor D1
#define tiltSensor D2
#define camLedOn D3
#define receiverWake D4
#define returnB D5
#define jackpotB D6 // 30 cents, 10% chance for now
#define snack1B D7
#define snack2B D8


//For interrupts
volatile unsigned long lastKnockTime = 0;
volatile unsigned long lastTiltTime = 0;
volatile unsigned long lastReturnTime = 0;
volatile unsigned long lastJackpotTime = 0;
volatile unsigned long lastSnack1Time = 0;
volatile unsigned long lastSnack2Time = 0;

volatile bool knockTriggered = false;
volatile bool tiltTriggered = false;
volatile bool returnTriggered = false;
volatile bool jackpotTriggered = false;
volatile bool snack1Triggered = false;
volatile bool snack2Triggered = false; 
volatile bool imageRev = false;
int debounceDelay = 400;//set same debounce delay for all buttons

//tam tam 20 cents
//mi~mi 20 cents
//pola original 30 cents
//pola seaweed 30 cents

volatile int snackSelected = 0; //1->11, 2->12, 5->jackpot(all)
volatile bool cancelled = false;

char MQTTstr[100] ="0";

//UART Mapping
//n (1-4): dispense snack n
//5 : dispense all (jackpot)
//6 : servo tilt to accept coins
//7 : servo tilt to reject coins
//8 : sound off buzzer
//9 : return coins to user

// char to store type casted snackSelected
char snackSelectedChar[4];

int expected_amount = 0;

void setup() {
  // put your setup code here, to run once
//  lcd.init();
  
  // Start the I2C Bus as Master
  Wire.begin();
  Serial.begin(9600);
  Serial1.begin(9600);
  initWifiandMQTT();
  
  pinMode(knockSensor,INPUT_PULLUP);
  pinMode(tiltSensor,INPUT_PULLUP); 
  pinMode(returnB,INPUT_PULLUP);
  pinMode(jackpotB,INPUT_PULLUP);
  pinMode(snack1B,INPUT_PULLUP);
  pinMode(snack2B,INPUT_PULLUP);
  pinMode(receiverWake,OUTPUT);// GPIO interrupt to wake up receiver
  digitalWrite(receiverWake,HIGH); //Set to high first, low will trigger interrupt to wake up
  pinMode(camLedOn,OUTPUT);
  
  //interrupt for all buttons for debouncing purposes
  attachInterrupt(digitalPinToInterrupt(knockSensor),knockInterrupt,FALLING); 
  attachInterrupt(digitalPinToInterrupt(tiltSensor),tiltInterrupt,FALLING); 
  attachInterrupt(digitalPinToInterrupt(returnB),returnInterrupt,FALLING);
  attachInterrupt(digitalPinToInterrupt(jackpotB),jackpotInterrupt,FALLING); 
  attachInterrupt(digitalPinToInterrupt(snack1B),snack1Interrupt,FALLING); 
  attachInterrupt(digitalPinToInterrupt(snack2B),snack2Interrupt,FALLING); 
}

//knock sensor interrupt
IRAM_ATTR void knockInterrupt() {
  int currTime = millis();
  if(currTime - lastKnockTime > debounceDelay){
    knockTriggered = true;
    lastKnockTime = currTime;
  }
}

//tilt button interrupt
IRAM_ATTR void tiltInterrupt() {
  int currTime = millis();
  if(currTime - lastTiltTime > debounceDelay){
    tiltTriggered = true; 
    lastTiltTime = currTime;
  }
}

// return button interrupt
IRAM_ATTR void returnInterrupt() {
  int currTime = millis();
  if(currTime - lastReturnTime > debounceDelay){
    returnTriggered = true; 
    cancelled = true;
    //act as cancel button when selecting snacks
    snackSelected = 0;
    lastReturnTime = currTime;
  }
}

//jackpot button interrupt
IRAM_ATTR void jackpotInterrupt() {
  int currTime = millis();
  if(currTime - lastJackpotTime  > debounceDelay){
    jackpotTriggered = true;
    lastJackpotTime = currTime;
  }
}

//snack 1 button interrupt
IRAM_ATTR void snack1Interrupt() {
  int currTime = millis();
  if(currTime - lastSnack1Time > debounceDelay){
    snack1Triggered = true; 
    lastSnack1Time = currTime;
  }
}

//snack 2 button interrupt
IRAM_ATTR void snack2Interrupt() {
  int currTime = millis();
  if(currTime - lastSnack2Time > debounceDelay){
    snack2Triggered = true; 
    lastSnack2Time = currTime;
  }
}


void payment(){
  knockTriggered = false;
  int current_amount = 0;
  switch(snackSelected){
    case 201:
      expected_amount = 20;
      Serial.write("101"); 
      Serial.write("106");
      break;
    case 202:
      expected_amount = 20;
      Serial.write("102");
      Serial.write("106");
      break;
//    case 3:
//      Serial.println(snackSelected);
//      expected_amount = 30;
//      break;
//    case 4:
//      Serial.println(snackSelected);
//      expected_amount = 30;
//      break;
    case 205:
      expected_amount = 30;
      Serial.write("105");
      Serial.write("107");
      break;
  } 
//  Serial.print("Selected snack: ");
//  Serial.println(snackSelected);
//  Serial.print("Please pay the total amount of: ");
//  Serial.println(expected_amount);

  do {
    if(knockTriggered == true) {
//      Serial.println("knock triggered");

      int amount_remaining = expected_amount - current_amount;
      if(amount_remaining == 10){
        Serial.write("117");
      } else if(amount_remaining == 20){
        Serial.write("118");
      } else {
        Serial.write("119");
      }
      //either snack or jackpot selected
      if(snackSelected != 0){
        //turn on camera LED
        digitalWrite(camLedOn, HIGH);
        
        //send to server to tell rpi camera to take photo
        send_coin_command();
        while(imageRev==false) {
          mqttClient.poll();
        }
        imageRev=false;
        
        //turn off camera LED
        digitalWrite(camLedOn, LOW);
        
        int valueFromML = String(MQTTstr).toInt();
//        Serial.print("Value from ML: ");
//        Serial.println(valueFromML);
        if(valueFromML == 10){
          Serial.write("108");
        } else if(valueFromML == 20){
          Serial.write("109");
        } else{
          Serial.write("110");
        }

        if(valueFromML == 10 || valueFromML == 20){
          current_amount += valueFromML;
          //send message to slave servo, tilt to accept
          digitalWrite(receiverWake,LOW);
          digitalWrite(receiverWake,HIGH);
          Serial.write("206");
        }
        else{
          //send message to slave servo, tilt to reject
          digitalWrite(receiverWake,LOW);
          digitalWrite(receiverWake,HIGH);
          Serial.write("207");
        }
      }
      // reset flag
      knockTriggered = false;
    }
    yield();
  } while(current_amount < expected_amount && !cancelled);
  
  if(cancelled == true){
     Serial.write("111");
  } else if (jackpotTriggered && random(1,101) > 90){
      //selected jackpot but fail to win. Chance of winning:90%
//      Serial.println("Failed to win Jackpot");
      Serial.write("112");

  } else {
      //paid full amt, release snacks
      if(snackSelected == 205){
//        Serial.println("Gong Xi Fa Cai!!");
        Serial.write("113");
      } else {
//        Serial.println("Snack dispensed");
        Serial.write("114");
      }
      dispense(snackSelected);
   }

  //Reset variables
  snackSelected = 0;
  cancelled = false;
  expected_amount = 0;
}

void dispense(int snackCode){
  //Sends message to slave, dispense corresponding snacks
  //Converts to char for UART transmission
  String temp_str = String(snackCode);
  temp_str.toCharArray(snackSelectedChar,4); 
  Serial.write(snackSelectedChar);  //this error
  Serial.write("115");
  
}

// Initialise WIFI and MQTT
void initWifiandMQTT(){
  WiFi.begin(ssid, password);
//  Serial.println("");
  pinMode(LED_BUILTIN, OUTPUT);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
//  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//    Serial.println("You're connected to the network");
//  Serial.println();
//
//  Serial.print("Attempting to connect to the MQTT broker: ");
//  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
//    Serial.print("MQTT connection failed! Error code = ");
//    Serial.println(mqttClient.connectError());

    while (1);
  }
//  Serial.println("You're connected to the MQTT broker!");
//  Serial.println();
   // set the message receive callback
  mqttClient.onMessage(onMqttMessage);
//  Serial.print("Subscribing to topic: WEMOS/testdone");
  mqttClient.subscribe(sub_topic);
//  Serial.print("Subscribing to topic: WEMOS/dispense");
  mqttClient.subscribe(sub_dispense);
//  Serial.print("Waiting for messages on topic: ");
//  Serial.println(sub_topic);
//  Serial.println();
//  Serial.print("Waiting for messages on topic: ");
//  Serial.println(sub_dispense);
//  Serial.println();
}

// Process incoming MQTT Message
void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
//  Serial.println("Received a message with topic '");
//  Serial.print(mqttClient.messageTopic());
//  Serial.print("', length ");
//  Serial.print(messageSize);
//  Serial.println(" bytes:");
  
  int i=0;
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    MQTTstr[i] = (char)mqttClient.read();
    ++i;
//    Serial.print((char)mqttClient.read());
  }
  MQTTstr[i] = '\0';

//  Serial.print(MQTTstr);
//  Serial.println();
  imageRev = true;

  int tele_cmd = String(MQTTstr).toInt();
  // snack 1
  if(tele_cmd == 1){
    Serial.write(100);
//    Serial.println("TELEGRAM Snack 1 dispensed");
  }
  // snack 2
  if(tele_cmd == 2){
    Serial.write(200);
//    Serial.println("TELEGRAM Snack 2 dispensed");
  }
  // jackpot
  if(tele_cmd == 3){
    Serial.write(500);
//    Serial.println("TELEGRAM Jackpot dispensed");
  }
}

// MQTT Take Picture Command
void send_coin_command(){
    int Rvalue = 123;
//    Serial.print("Sending message to topic: ");
//    Serial.println(pub_topic);
//    Serial.println(Rvalue);
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(pub_topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();
//    Serial.println();
}

// Publish to Server if machine stolen
void stolen_machine(){
//    int Svalue = 123;
//    Serial.print("Sending message to topic: ");
//    Serial.println(pub_stolen);
//    Serial.println(Svalue);
//    // send message, the Print interface can be used to set the message contents
//    mqttClient.beginMessage(pub_stolen);
//    mqttClient.print(Svalue);
//    mqttClient.endMessage();
//    Serial.println();
    int Rvalue = 6969;
//    Serial.print("Sending message to topic: ");
//    Serial.println(pub_topic);
//    Serial.println(Rvalue);
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(pub_topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();
//    Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  mqttClient.poll();
  if(tiltTriggered) {
//    Serial.println("tilt triggered");
    Serial.write("116");
    // send message to server to tell tilt switch triggered, server to phone 
    stolen_machine();
//    send_coin_command();  
    // Send message to slave, sound off buzzer 
    digitalWrite(receiverWake,LOW);
    digitalWrite(receiverWake,HIGH);
    Serial.write("208");        

    // disable all interrupts for 1 minute
    noInterrupts();
    delay(60000);
  
    // enable interrupt 
    interrupts();
    
    // reset flag
    tiltTriggered = false;
  }
  else if(returnTriggered) {
//    Serial.println("return pressed");
    //Send message to slave, return coins in holding area
    digitalWrite(receiverWake,LOW);
    digitalWrite(receiverWake,HIGH);
    Serial.write("209"); 
    
    // reset flag
    returnTriggered = false;
  }
  else if(jackpotTriggered) {
    //disable jackpot when selecting snacks

    snackSelected = 205;
    payment();
    
    // reset variables
    jackpotTriggered = false;
  }
  else if(snack1Triggered) {
//    Serial.println("snack 1 pressed");
    cancelled = false;
    snackSelected = 201;
    payment();
    snack1Triggered = false;
  }
  else if(snack2Triggered) {
//    Serial.println("snack 2 pressed");
    cancelled = false;
    snackSelected = 202;
    payment();
    snack2Triggered = false;
  }
  else{
    //Serial.println("no");
  }
}
