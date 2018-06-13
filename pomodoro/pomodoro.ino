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

String _audio = "", activities[5];
bool recorded = false;
int seconds = 0, minutes = 0, count, side = 5;
long startingMillis = 0, timer[5] = {1500000, 15000, 1500000, 1500000, 1500000}, delta[5] = {0,0,0,0,0};
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
  JSONencoder["delta"] = timeSpent;
    
  String json;
  JSONencoder.printTo(json);
  return json;
}

JsonArray& parseStringToJSON(String json){
  DynamicJsonBuffer JSONbuffer;
  JsonArray& parsed = JSONbuffer.parseArray(json);
  return parsed;
}

void sendJSONAudio(String json) {
  if(WiFiMulti.run() == WL_CONNECTED ) {
    http.begin("http://smartpomodoro-backend2-smartpomodoro.1d35.starter-us-east-1.openshiftapps.com/api/pomodoroserver");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Basic Y2Fzc29sOm9maWNpbmFzMw==");

    int httpCode = http.POST(json);
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
  }
}

void sendJSONActivity(String json) {
  if(WiFiMulti.run() == WL_CONNECTED ) {
    http.begin("http://smartpomodoro-backend2-smartpomodoro.1d35.starter-us-east-1.openshiftapps.com/api/activity");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Basic Y2Fzc29sOm9maWNpbmFzMw==");

    int httpCode = http.POST(json);
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
  }
}

JsonArray& getFaces() { 
  if(WiFiMulti.run() == WL_CONNECTED ) {
    http.begin("http://smartpomodoro-backend2-smartpomodoro.1d35.starter-us-east-1.openshiftapps.com/api/userfield/faces");
    http.addHeader("Authorization", "Basic Y2Fzc29sOm9maWNpbmFzMw==");

    int httpCode = http.GET();
    if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    Serial.println("\n\nPAYLOAD");
    String payload = http.getString();
    Serial.println(payload);
    http.end();

    return parseStringToJSON(payload);
  } else{
    Serial.println("Error in WiFi connection");
  }
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
  if(sideNumber == 0){
    lcd1.noDisplay();
    lcd1.noBacklight();
    lcd2.noDisplay();
    lcd2.noBacklight();
    lcd3.noDisplay();
    lcd3.noBacklight();
    lcd4.noDisplay();
    lcd4.noBacklight();
  }else if(sideNumber == 1){
    lcd0.noDisplay();
    lcd0.noBacklight();
    lcd2.noDisplay();
    lcd2.noBacklight();
    lcd3.noDisplay();
    lcd3.noBacklight();
    lcd4.noDisplay();
    lcd4.noBacklight();
  }else if(sideNumber == 2){
    lcd0.noDisplay();
    lcd0.noBacklight();
    lcd1.noDisplay();
    lcd1.noBacklight();
    lcd3.noDisplay();
    lcd3.noBacklight();
    lcd4.noDisplay();
    lcd4.noBacklight();
  }else if(sideNumber == 3){
    lcd0.noDisplay();
    lcd0.noBacklight();
    lcd1.noDisplay();
    lcd1.noBacklight();
    lcd2.noDisplay();
    lcd2.noBacklight();
    lcd4.noDisplay();
    lcd4.noBacklight();
  }else if(sideNumber == 4){
    lcd0.noDisplay();
    lcd0.noBacklight();
    lcd1.noDisplay();
    lcd1.noBacklight();
    lcd2.noDisplay();
    lcd2.noBacklight();
    lcd3.noDisplay();
    lcd3.noBacklight();
  }else{
    lcd0.noDisplay();
    lcd0.noBacklight();
    lcd1.noDisplay();
    lcd1.noBacklight();
    lcd2.noDisplay();
    lcd2.noBacklight();
    lcd3.noDisplay();
    lcd3.noBacklight();
    lcd4.noDisplay();
    lcd4.noBacklight();
  }
}
 
void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(D0, INPUT);
  pinMode(D7, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D8, OUTPUT);
  pinMode(D9, OUTPUT);
  pinMode(D10, OUTPUT);
  //pinMode(9, OUTPUT);
  //pinMode(26, OUTPUT);

  lcd0.init();
  lcd1.init();
  lcd2.init();
  lcd3.init();
  lcd4.init();
  
  turnLcdOff(5);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Pomodoro", "123123123");

  Serial.print("Connecting");
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print('.');
  }

  JsonArray& faces = getFaces();

  activities[0] = faces.get<String>(0);
  activities[1] = faces.get<String>(1);
  activities[2] = faces.get<String>(2);
  activities[3] = faces.get<String>(3);
  activities[4] = faces.get<String>(4);
}
 
void loop(){
    readAccel();
    recorded = false;
  
    //Serial.println("D0: " + String(digitalRead(D0)) + " D4: " + String(digitalRead(D4)) + " D5: " + String(digitalRead(D5)) + " D6: " + String(digitalRead(D6)) + " D7: " + String(digitalRead(D7)));
    if( xAxis < -13000 ){
      if (side != 0) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        startingMillis = millis();  
        side = 0;
        turnLcdOff(side);
      }
      
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
      lcd0.print(activities[side]); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd0.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd0.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD
      
      if (minutes == 0 && seconds == 0) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        delta[side] = 0;
        
        Serial.println("acende led");
        digitalWrite(D10, HIGH);
        
        if(digitalRead(D0) == 1) {
          while(digitalRead(D0) == 1) {
            ESP.wdtDisable();
            ESP.wdtEnable(1000);
          }
        }
        
        Serial.println("apaga led");
        digitalWrite(D10, LOW);
        
        delay(1000);
        startingMillis = millis();
      }
      
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

      //if(recorded) sendJSON(makeJSONAudio(side, _audio, frequency));
       
      _audio = "";
      recorded = false;

      
    } else if ( yAxis < -13000 ) {
      if (side != 1) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        startingMillis = millis();  
        side = 1;
        turnLcdOff(side);
      }
      
      long count = 0;
      if (millis() - startingMillis >= 1000 ) {
        delta[side] += millis() - startingMillis;
        startingMillis = millis();
      }
      seconds = ((timer[side] - delta[side]) / 1000) % 60;
      minutes = (((timer[side] - delta[side]) / 1000) / 60) % 60;

      lcd1.init();
      lcd1.display();   // INICIALIZA O DISPLAY LCD
      lcd1.backlight();
      lcd1.home();
      lcd1.print(activities[side]); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd1.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd1.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD

      if (minutes == 0 && seconds == 0) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        delta[side] = 0;
        
        Serial.println("acende led");
        digitalWrite(D9, HIGH);
        
        if(digitalRead(D4) == 1) {
          while(digitalRead(D4) == 1) {
            ESP.wdtDisable();
            ESP.wdtEnable(1000);
          }
        }
        
        Serial.println("apaga led");
        digitalWrite(D9, LOW);
        
        delay(1000);
        startingMillis = millis();
      }
      
      long acquiringTime = millis();
      /*while(digitalRead(D4) != 1){
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
      
    } else if ( yAxis > 13000 ) {
      if (side != 4) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        startingMillis = millis();  
        side = 4;
        turnLcdOff(side);
      }

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
      lcd4.print(activities[side]); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd4.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd4.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD

      if (minutes == 0 && seconds == 0) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        delta[side] = 0;
        
        Serial.println("acende led");
        //digitalWrite(9, HIGH);
        
        if(digitalRead(D6) == 1) {
          while(digitalRead(D6) == 1) {
            ESP.wdtDisable();
            ESP.wdtEnable(1000);
          }
        }
        
        Serial.println("apaga led");
        //digitalWrite(9, LOW);
        
        delay(1000);
        startingMillis = millis();
      }
      
      long acquiringTime = millis();
      /*while(digitalRead(D6) != 1){
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
      
    } else if ( zAxis > 13000 ) {
      if (side != 3) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        startingMillis = millis();  
        side = 3;
        turnLcdOff(side);
      }
      
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
      lcd3.print(activities[side]); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd3.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd3.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD

      if (minutes == 0 && seconds == 0) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        delta[side] = 0;
        
        Serial.println("acende led");
        //digitalWrite(26, HIGH);
        
        if(digitalRead(D5) == 1) {
          while(digitalRead(D5) == 1) {
            ESP.wdtDisable();
            ESP.wdtEnable(1000);
          }
        }
        
        Serial.println("apaga led");
        //digitalWrite(26, LOW);
        
        delay(1000);
        startingMillis = millis();
      }
      
      long acquiringTime = millis();
      /*while(digitalRead(D5) != 1){
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
      
     // if(recorded) sendJSON(makeJSONAudio(side, _audio, frequency));    
       
      _audio = "";
      recorded = false;
      
    } else if ( zAxis < -13000 ) {
      if (side != 2) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        startingMillis = millis(); 
        side = 2;
        turnLcdOff(side); 
      }
      
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
      lcd2.print(activities[side]); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd2.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd2.print(String(minutes) + ":" + String(seconds)); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD

      if (minutes == 0 && seconds == 0) {
        sendJSONActivity(makeJSONActivity(side, delta[side]));
        delta[side] = 0;
        
        Serial.println("acende led");
        digitalWrite(D8, HIGH);
        
        if(digitalRead(D7) == 1) {
          while(digitalRead(D7) == 1) {
            ESP.wdtDisable();
            ESP.wdtEnable(1000);
          }
        }
        
        Serial.println("apaga led");
        digitalWrite(D8, LOW);
        
        delay(1000);
        startingMillis = millis();
      }
      
      long acquiringTime = millis();
      /*while(digitalRead(D7) != 1){
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
      
      //if (recorded) sendJSON(makeJSONAudio(side, _audio, frequency));
      
      _audio = "";
      recorded = false;
      
    } else if ( xAxis > 3000 ) {
      if (side != 5) {
        sendJSONActivity(makeJSONActivity(side, delta[side])); 
        side = 5;
        turnLcdOff(side);  
      } 
    }
}

