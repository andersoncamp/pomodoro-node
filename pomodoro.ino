#include <Wire.h>  //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal_I2C.h> //INCLUSÃO DE BIBLIOTECA

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
 
//LiquidCrystal_I2C* lcds = (LiquidCrystal_I2C*)malloc(sizeof(LiquidCrystal_I2C)*5); //= {(0x3F, 16, 2),(0x3E, 16, 2),(0x3D, 16, 2),(0x3C, 16, 2),(0x3B, 16, 2),}; //FUNÇÃO DO TIPO "LiquidCrystal_I2C"
LiquidCrystal_I2C lcd0(0x3F, 16, 2),lcd1(0x3E, 16, 2),lcd2(0x3D, 16, 2),lcd3(0x3C, 16, 2),lcd4(0x3B, 16, 2);

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;

String audio = "";
bool recorded = false;
int count;


void sendAudio(String lcd_number) {
  if(WiFiMulti.run() == WL_CONNECTED && recorded) {
    http.begin("192.168.25.89", 8081);
    //http.begin("192.168.1.12", 80, "/test.html");

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
  lcd0.init();
  lcd0.noDisplay();
  lcd0.noBacklight();
  lcd1.init();
  lcd1.noDisplay();
  lcd1.noBacklight();
  lcd2.init();
  lcd2.noDisplay();
  lcd2.noBacklight();
  lcd3.init();
  lcd3.noDisplay();
  lcd3.noBacklight();
  lcd4.init();
  lcd4.noDisplay();
  lcd4.noBacklight();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("XXXXXX", "XXXXXXXX");

  // allow reuse (if server supports it)
  http.setReuse(true);
}
 
void loop(){
    recorded = false;
    //Serial.println("D0:" + String(digitalRead(D0)) + "D8:" + String(digitalRead(D8)) +"D4:" + String(digitalRead(D4)) + "D5:" + String(digitalRead(D5)) + "D6:" + String(digitalRead(D6)));
    if(digitalRead(D0) != 1){
      lcd0.display();   // INICIALIZA O DISPLAY LCD
      lcd0.backlight(); 
      lcd0.home();
      lcd0.print("BT 0 PRESSIONADO"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd0.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd0.print("GRAVANDO..."); //ESCREVE O TEXTO NA SEGUNDA LINHA DO DISPLAY LCD
      long count = 0;
      while(digitalRead(D0) != 1 && count < 30){
        for(byte i = 0; i < 128; i++) {
          audio = audio + String(analogRead(A0), HEX) + ",";
        }
        recorded = true;
        count++;
      };
      sendAudio("0");
      audio = "";
      recorded = false;
      lcd0.noDisplay();
      lcd0.noBacklight();
    } else if (digitalRead(D8) != 1) {
  
      lcd1.display();   // INICIALIZA O DISPLAY LCD
      lcd1.backlight(); 
      lcd1.home();
      lcd1.print("BT 1 PRESSIONADO"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd1.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd1.print("GRAVANDO...");
      long count = 0;
      while(digitalRead(D8) != 1 && count < 30){
        for(byte i = 0; i < 128; i++) {
          audio = audio + String(analogRead(A0), HEX) + ",";
        }
        recorded = true;
        count++;
      };
      sendAudio("1");
      audio = "";
      recorded = false;
      lcd1.noDisplay();
      lcd1.noBacklight();
    } else if (digitalRead(D4) != 1) {
      
      lcd2.display();   // INICIALIZA O DISPLAY LCD
      lcd2.backlight(); 
      lcd2.home();
      lcd2.print("BT 2 PRESSIONADO"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd2.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd2.print("GRAVANDO...");
      long count = 0;
      while(digitalRead(D4) != 1 && count < 30){
        for(byte i = 0; i < 128; i++) {
          audio = audio + String(analogRead(A0), HEX) + ",";
        }
        recorded = true;
        count++;
      };
      sendAudio("2");
      audio = "";
      recorded = false;
      lcd2.noDisplay();
      lcd2.noBacklight();
    } else if (digitalRead(D5) != 1) {
      
      lcd3.display();   // INICIALIZA O DISPLAY LCD
      lcd3.backlight(); 
      lcd3.home();
      lcd3.print("BT 3 PRESSIONADO"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd3.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd3.print("GRAVANDO...");
      long count = 0;
      while(digitalRead(D5) != 1 && count < 30){
        for(byte i = 0; i < 128; i++) {
          audio = audio + String(analogRead(A0), HEX) + ",";
        }
        recorded = true;
        count++;
      };
      sendAudio("3");
      audio = "";
      recorded = false;
      lcd3.noDisplay();
      lcd3.noBacklight();
    } else if (digitalRead(D6) != 1) {
      
      lcd4.display();   // INICIALIZA O DISPLAY LCD
      lcd4.backlight(); 
      lcd4.home();
      lcd4.print("BTN 4 PRESSIONADO"); //ESCREVE O TEXTO NA PRIMEIRA LINHA DO DISPLAY LCD
      lcd4.setCursor(0, 1); //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)      
      lcd4.print("GRAVANDO...");
      long count = 0;
      while(digitalRead(D6) != 1 && count < 30){
        for(byte i = 0; i < 128; i++) {
          audio = audio + String(analogRead(A0), HEX) + ",";
        }
        recorded = true;
        count++;
      };
      sendAudio("4");
      audio = "";
      recorded = false;
      lcd4.noDisplay();
      lcd4.noBacklight();
    }
}

