#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd0(0x3F, 16, 2),lcd1(0x3A, 16, 2),lcd2(0x3D, 16, 2),lcd3(0x3C, 16, 2),lcd4(0x3B, 16, 2);

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
const int latch_addr=0x3E;  // I2C address of the PCF8574A

String audio = "";
bool recorded = false;
int seconds = 0, minutes = 0, count, side = 5;
long startingMillis = 0, timer[5] = {1500000, 1500000, 1500000, 1500000, 1500000}, delta[5] = {0,0,0,0,0};
int16_t xAxis=0, yAxis=0, zAxis=0;
byte power = 0;
    
void setupAccel(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void readAccel(){
  setupAccel();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6,true);  // request a total of 6 registers
  xAxis=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  yAxis=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  zAxis=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
}

void turnLcdOff(int sideNumber) {
  if(sideNumber == 0){
    power = 0x01;
  }else if(sideNumber == 1){
    power = 0x02;
  }else if(sideNumber == 2){
    power = 0x04;
  }else if(sideNumber == 3){
    power = 0x08;
  }else if(sideNumber == 4){
    power = 0x10;
  }else{
    power = 0x00;
  }

  Wire.beginTransmission(latch_addr);
  Wire.write(power);
  Wire.endTransmission(false);
}

void sendAudio(int sideNumber) {
  if(WiFiMulti.run() == WL_CONNECTED && recorded) {
    http.begin("192.168.25.89", 8081);

    int httpCode = http.POST(audio);
    if(httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
 
void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(D0, INPUT);
  pinMode(D8, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  
  turnLcdOff(5);

  //WiFi.mode(WIFI_STA);
  //WiFiMulti.addAP("XXXXXX", "XXXXXXXX");

  // allow reuse (if server supports it)
  http.setReuse(true);
}
 
void loop(){
    readAccel();
    recorded = false;
    
    if(digitalRead(D0) != 1 || xAxis > 3000 && yAxis < 3000 ){
      if (side != 0) {
        startingMillis = millis();  
      }
      side = 0;
      turnLcdOff(side);
      
      lcd1.init();
      lcd1.display();   // INICIALIZA O DISPLAY LCD
      lcd1.backlight();
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;
      lcd1.home();
      lcd1.print("ATIVIDADE 0"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd1.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd1.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD

      long acquiringTime = millis();
      while(digitalRead(D0) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          audio = audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      //sendAudio(side);
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      Serial.println("Hertz: " + String(frequency));
      Serial.println(audio);
      audio = "";
      recorded = false;
    } else if (digitalRead(D8) != 1 || xAxis < 3000 && yAxis < 1000 ) {
      if (side != 1) {
        startingMillis = millis();  
      }
      side = 1;
      turnLcdOff(side);

      lcd1.init();
      lcd1.display();   // INICIALIZA O DISPLAY LCD
      lcd1.backlight();
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;
      lcd1.home();
      lcd1.print("ATIVIDADE 1"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd1.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd1.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD
      
      long acquiringTime = millis();
      while(digitalRead(D0) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          audio = audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      //sendAudio(side);
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      Serial.println("Hertz: " + String(frequency));
      Serial.println(audio);
      audio = "";
      recorded = false;
      
    } else if (digitalRead(D4) != 1 || yAxis > 0 && zAxis > 0) {
      if (side != 4) {
        startingMillis = millis();  
      }
      side = 4;
      turnLcdOff(side);

      lcd4.init();
      lcd4.display();   // INICIALIZA O DISPLAY LCD
      lcd4.backlight();
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;
      lcd4.home();
      lcd4.print("ATIVIDADE 4"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd4.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd4.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD
      
      long acquiringTime = millis();
      while(digitalRead(D0) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          audio = audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      //sendAudio(side);
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      Serial.println("Hertz: " + String(frequency));
      Serial.println(audio);
      audio = "";
      recorded = false;
      
    } else if (digitalRead(D5) != 1 || yAxis < 0 && zAxis < 0) {
      if (side != 3) {
        startingMillis = millis();  
      }
      side = 3;
      turnLcdOff(side);
      
      lcd3.init();
      lcd3.display();   // INICIALIZA O DISPLAY LCD
      lcd3.backlight();
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;
      lcd3.home();
      lcd3.print("ATIVIDADE 3"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd3.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd3.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD
      
      long acquiringTime = millis();
      while(digitalRead(D0) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          audio = audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      //sendAudio(side);
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      Serial.println("Hertz: " + String(frequency));
      Serial.println(audio);
      audio = "";
      recorded = false;
      
    } else if (digitalRead(D6) != 1 || zAxis > 0 && yAxis > 0) {
      if (side != 2) {
        startingMillis = millis();  
      }
      side = 2;
      turnLcdOff(side);
      
      lcd2.init();
      lcd2.display();   // INICIALIZA O DISPLAY LCD
      lcd2.backlight();
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;
      lcd2.home();
      lcd2.print("ATIVIDADE 2"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd2.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd2.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD
      
      long acquiringTime = millis();
      while(digitalRead(D0) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          audio = audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      //sendAudio(side);
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      Serial.println("Hertz: " + String(frequency));
      Serial.println(audio);
      audio = "";
      recorded = false;
      
    } else /*if ( zAxis < 0 && yAxis < 1000 )*/ {
      side = 5;
      turnLcdOff(side);  
    }
}

