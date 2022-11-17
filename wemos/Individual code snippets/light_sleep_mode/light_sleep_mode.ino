/*
 * written by xing yuan 
 * This code snippet uses a button to trigger a GPIO interrupt 
 * to wake a WeMos D1 from light sleep mode
 *  
 * code is modified based on studying the examples from the sources below
 * https://www.mischianti.org/2019/11/21/wemos-d1-mini-esp8266-the-three-type-of-sleep-mode-to-manage-energy-savings-part-4/#Light_sleep_GPIO_wake_up
 * https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/LowPowerDemo/LowPowerDemo.ino
 */

volatile unsigned long lastStartTime = 0;
volatile bool startTriggered = false;
int debounceDelay = 50;
#define buttonPinD3 D3
#define LIGHT_WAKE_PIN D3

#include "Arduino.h"
#include <ESP8266WiFi.h>
 
#define FPM_SLEEP_MAX_TIME           0xFFFFFFF
 
// Required for LIGHT_SLEEP_T delay mode
extern "C" {
#include "user_interface.h"
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPinD3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPinD3), pressed, FALLING);
}

IRAM_ATTR void pressed() {
  int currTime = millis();
  if(currTime - lastStartTime > debounceDelay){
    startTriggered = true; 
    lastStartTime = currTime;
  }
}

void callback() {
  Serial.println("Callback");
  Serial.flush();
}

void loop() {
  Serial.println("Enter light sleep mode");
 
  //GPIO wake up
  gpio_pin_wakeup_enable(GPIO_ID_PIN(LIGHT_WAKE_PIN), GPIO_PIN_INTR_LOLEVEL);
  wifi_set_opmode(NULL_MODE);
  //sleep mode
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(callback);
  wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
  delay(1000);

  //insert code between here and
  if(startTriggered == true){
    Serial.println("pressed");
    startTriggered = false;
  }
  Serial.println("Exit light sleep mode");
 //here 
  wifi_set_sleep_type(NONE_SLEEP_T);
  delay(1000);  //  Put the esp to sleep for 15s


}
