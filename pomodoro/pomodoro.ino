#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd0(0x3F, 16, 2),lcd1(0x3A, 16, 2),lcd2(0x3D, 16, 2),lcd3(0x3C, 16, 2),lcd4(0x3B, 16, 2);

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
const int latch_addr=0x3E;  // I2C address of the PCF8574A

String _audio = "";
bool recorded = false;
int seconds = 0, minutes = 0, count, side = 5;
long startingMillis = 0, timer[5] = {1500000, 1500000, 1500000, 1500000, 1500000}, delta[5] = {0,0,0,0,0};
int16_t xAxis=0, yAxis=0, zAxis=0;
byte power = 0;
String jsonFull;

String makeJSONAudio(int faceID, String audio, unsigned long frequencia){
  const size_t bufferSize = JSON_OBJECT_SIZE(3);
  DynamicJsonBuffer JSONbuffer(bufferSize);
  JsonObject& JSONencoder = JSONbuffer.createObject(); 
  
  JSONencoder["faceID"] = faceID;
  JSONencoder["audio"] = audio;
  JSONencoder["frequencia"] = frequencia;
    
  String json;
  JSONencoder.printTo(json);
  return json;
}

String makeJSONActivity(int faceID, unsigned long timeSpent){  
  const size_t bufferSize = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer JSONbuffer(bufferSize);
  JsonObject& JSONencoder = JSONbuffer.createObject(); 
  
  JSONencoder["faceID"] = faceID;
  JSONencoder["timeSpent"] = timeSpent;
    
  String json;
  JSONencoder.printTo(json);
  return json;
}

void sendJSON(String json) {

//  if(WiFi.status() == WL_CONNECTED) {
  /*if(WiFiMulti.run() == WL_CONNECTED ) {
    http.begin("http://smartpomodoro-backend2-smartpomodoro.1d35.starter-us-east-1.openshiftapps.com/api/pomodoroserver");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(json);
//    int httpCode = http.GET();
    if(httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    Serial.println("\n\nPAYLOAD");
    String payload = http.getString();
    Serial.println(payload);

    http.end();
  } else{
    Serial.println("Error in WiFi connection");
  }*/
}
    
void setupAccel(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
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
  /*if(sideNumber == 0){
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
  }*/

  power = 0xFF;

  Wire.beginTransmission(latch_addr);
  Wire.write(power);
  Wire.endTransmission(false);
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

  // allow reuse (if server supports it)
 // http.setReuse(true);
}
 
void loop(){
    readAccel();
    recorded = false;

  
    
    if(digitalRead(D0) == 1 ||  xAxis > 3000 && yAxis < 3000 ){
      if (side != 0) {
        //sendJSON(makeJSONActivity(side, delta[side]));
        startingMillis = millis();  
      }
      side = 0;
      turnLcdOff(side);
      
      lcd0.init();
      lcd0.display();   // INICIALIZA O DISPLAY LCD
      lcd0.backlight();
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;
      lcd0.home();
      lcd0.print("ATIVIDADE 0"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd0.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd0.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD

      long acquiringTime = millis();
      /*while(digitalRead(D0) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          _audio = _audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };*/
      
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));

      if(recorded) sendJSON(makeJSONAudio(side, _audio, frequency));
       
      _audio = "";
      recorded = false;

      Serial.println("we here");

      
    } else /*if (digitalRead(D4) == 1 || xAxis < 3000 && yAxis < 1000 ) */{
      if (side != 1) {
        //sendJSON(makeJSONActivity(side, delta[side]));
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
      /*while(digitalRead(D8) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          _audio = _audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };*/
      
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      //if(recorded) sendJSON(makeJSONAudio(side, _audio, frequency)); 
       
      _audio = "";
      recorded = false;
      
    } /*else if (digitalRead(D6) != 1 || yAxis > 0 && zAxis > 0) {
      if (side != 4) {
        sendJSON(makeJSONActivity(side, delta[side]));
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
      while(digitalRead(D4) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          _audio = _audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));

      //if(recorded) sendJSON(makeJSONAudio(side, _audio, frequency));
       
      _audio = "";
      recorded = false;
      
    } else if (digitalRead(D5) != 1 || yAxis < 0 && zAxis < 0) {
      if (side != 3) {
        sendJSON(makeJSONActivity(side, delta[side]));
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
      while(digitalRead(D5) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          _audio = _audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };

      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      
     // if(recorded) sendJSON(makeJSONAudio(side, _audio, frequency));    
       
      _audio = "";
      recorded = false;
      
    } else if (digitalRead(D6) != 1 || zAxis > 0 && yAxis > 0) {
      if (side != 2) {
        sendJSON(makeJSONActivity(side, delta[side]));
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
      while(digitalRead(D6) != 1){
        ESP.wdtDisable();
        
        for(byte i = 0; i < 128; i++) {
          int aux = analogRead(A0);
          _audio = _audio + String(aux, HEX) + ",";
        }
        
        ESP.wdtEnable(1000);
        recorded = true;
        count++;
      };
      
      int frequency = (int)count*128.00*1000/((millis() - acquiringTime));
      
      //if (recorded) sendJSON(makeJSONAudio(side, _audio, frequency));
      
      _audio = "";
      recorded = false;
      
    } else if ( true || zAxis < 0 && yAxis < 1000 ) {
      if (side != 5) {
        sendJSON(makeJSONActivity(side, delta[side]));  
      }
      side = 5;
      turnLcdOff(side);  
    }*/
}

