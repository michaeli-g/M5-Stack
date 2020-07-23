#include <M5Stack.h>
#include <Wire.h>
#include <WiFi.h>
#include <SD.h>
#include <analogWrite.h>
#include <Adafruit_INA219.h>
#include "config.h"
#define SPEAKER_PIN 25
#define PWM_PIN 5        // gotta change
#define R1 100000
#define R2 22000

AdafruitIO_Feed *vSoc = io.feed("Source Voltage");
AdafruitIO_Feed *vBatt = io.feed("Battery Voltage");
AdafruitIO_Feed *vGate = io.feed("Gate Voltage");
AdafruitIO_Feed *batP = io.feed("Battery Percentage");

int onVal = 0;
float vG = 0;
float aG = 0;
int count = 0;


Adafruit_INA219 ina219;

void setup() {

  Wire.begin();
  // put your setup code here, to run once:
  M5.begin(true, true, true, false);
  Serial.begin(115200);
  dacWrite(25,0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(3);
  
  pinMode(PWM_PIN, OUTPUT);

//adafruit-------------------------------------------
  //connect to io.adafruit.com
  io.connect();
  Serial.print("Connecting to Adafruit IO");

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
//adafruit-------------------------------------------

//curr sensor----------------------------------------
  uint32_t currentFrequency;

 // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
//  if (! ina3221.begin()) {
//    //Serial.println("Failed to find INA219 chip");
//    while (1) { delay(10); }
//  }
  //ina219.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  //Serial.println("Measuring voltage and current with INA219 ...");
//curr sensor---------------------------------------

}

void loop() {

  io.run();
  
  // put your main code here, to run repeatedly:
  float v1 = (float)analogRead(36);
  float v2 = 0;
  
   if(v1 < 5){       
      v2 = 0;
  }
  else if(v1 <= 1084){ 
      v2 = 0.11 + (0.89 / 1084) * v1;
  }
  else if(v1 <= 2303){   
      v2 = 1.0 + (1.0 / (2303 - 1084)) * (v1 - 1084);
  }
  else if(v1 <= 3179){    
      v2 = 2.0 + (0.7 / (3179 - 2303)) * (v1 - 2303);
  }
  else if(v1 <= 3659){    
      v2 = 2.7 + (0.3 / (3659 - 3179)) * (v1 - 3179);
  }
  else if(v1 <= 4071){    
      v2 = 3.0 + (0.2 / (4071 - 3659)) * (v1 - 3659);
  }
  else{    
      v2 = 3.2;
  } 



//  float v2 = (float)analogRead(35) / 4096 * 17.4 * 0.975;
//  float current = (float)analogRead(5);

  float vS = (v2*(R1 + R2)) / R2;
  float diff = vS - vG;

  //Inverse voltage divider to get voltage going into battery
  //Ideal difference being outputted is 10v, minimum 4v.
  //pwm------------------------------------------

  if(diff > 10){
    vG += (diff - 10);
    aG = vG * 75;
  }
  /*else if(diff < 4){
    vG -= diff;
    aG = vG * 75;
  }*/
  else if(diff < 9.5){
    vG = .7;
    aG = vG * 75;
  }

  analogWrite(5, aG);
  //pwm-------------------------------------------

  //curr sensor-----------------------------------
//  float current_mA = 0;
//  current_mA = ina3221.getCurrent_mA(1);
  //Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  //curr sensor-----------------------------------

  float vDiv1 = (float)analogRead(35);
  float vDiv2 = 0;

  if(vDiv1 < 5){    
      vDiv2 = 0;
  }
  else if(vDiv1 <= 1084){ 
      vDiv2 = 0.11 + (0.89 / 1084) * v1;
  }
  else if(vDiv1 <= 2303){   
      vDiv2 = 1.0 + (1.0 / (2303 - 1084)) * (vDiv1 - 1084);
  }
  else if(vDiv1 <= 3179){    
      vDiv2 = 2.0 + (0.7 / (3179 - 2303)) * (vDiv1 - 2303);
  }
  else if(vDiv1 <= 3659){    
      vDiv2 = 2.7 + (0.3 / (3659 - 3179)) * (vDiv1 - 3179);
  }
  else if(vDiv1 <= 4071){    
      vDiv2 = 3.0 + (0.2 / (4071 - 3659)) * (vDiv1 - 3659);
  }
  else{    
      vDiv2 = 3.2;
  } 
  
  float vBat = (vDiv2    * (R1 + R2)) / R2;
  //12 - 14
  float Chrg = (vBat - 12) * 50;

  if(Chrg >= 100){
    Chrg = 100;
    vG = 0;       //turns off circuit if battery is fully charged
  }
  else if(Chrg <= 0){
    Chrg = 0;
  }
  

  count++;

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.print("V1: ");
  M5.Lcd.print(v2);
  M5.Lcd.print("V");

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 40);
  M5.Lcd.print("VS: ");
  M5.Lcd.print(vS);
  M5.Lcd.print("V");

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.print("VG: ");
  M5.Lcd.print(vG);
  M5.Lcd.print("V");

//  M5.Lcd.setTextColor(WHITE);
//  M5.Lcd.setCursor(10, 100);
//  M5.Lcd.print("Current: ");
//  M5.Lcd.print(current_mA);
//  M5.Lcd.print("mA");  

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 100);
  M5.Lcd.print("Charge: ");
  M5.Lcd.print(Chrg);
  M5.Lcd.print("%");

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 130);
  M5.Lcd.print("Count: ");
  M5.Lcd.print(count);

  delay(300);

  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.print("V1: ");
  M5.Lcd.print(v2);
  M5.Lcd.print("V");

  M5.Lcd.setCursor(10, 40);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.print("VS: ");
  M5.Lcd.print(vS);
  M5.Lcd.print("V");

  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.print("VG: ");
  M5.Lcd.print(vG);
  M5.Lcd.print("V");

//  M5.Lcd.setTextColor(BLACK);
//  M5.Lcd.setCursor(10, 100);
//  M5.Lcd.print("Current: ");
//  M5.Lcd.print(current_mA);
//  M5.Lcd.print("mA");

  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(10, 100);
  M5.Lcd.print("Charge: ");
  M5.Lcd.print(Chrg);
  M5.Lcd.print("%");

  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(10, 130);
  M5.Lcd.print("Count: ");
  M5.Lcd.print(count);


//  //adafruit-------------------------------------------
  if(count >= 150){
    Serial.print("sending -> ");
    Serial.println(vS);
    vSoc->save(vS);

    Serial.print("sending -> ");
    Serial.println(vBat);
    vBatt->save(vBat);

    Serial.print("sending -> ");
    Serial.println(vG);
    vGate->save(vG);

    Serial.print("sending -> ");
    Serial.println(Chrg);
    batP->save(Chrg);
    
    count = 0;
   }

//adafruit-------------------------------------------
  
}
