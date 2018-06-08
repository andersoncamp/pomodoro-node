#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;
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

String makeJSONActivity(int faceID, unsigned long timeStarted, unsigned long timeEnded){  
  const size_t bufferSize = JSON_OBJECT_SIZE(3);
  DynamicJsonBuffer JSONbuffer(bufferSize);
  JsonObject& JSONencoder = JSONbuffer.createObject(); 
  
  JSONencoder["faceID"] = faceID;
  JSONencoder["timeStarted"] = timeStarted;
  JSONencoder["timeEnded"] = timeEnded;
    
  String json;
  JSONencoder.printTo(json);
  return json;
}

void sendJSON(String json) {

//  if(WiFi.status() == WL_CONNECTED) {
  if(WiFiMulti.run() == WL_CONNECTED ) {
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
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000);
//  Serial.setDebugOutput(true);
  Serial.println();
//  http.setReuse(true);

//  WiFi.begin("------", "-----");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("------", "-------");

  Serial.print("Connecting");
//  while (WiFi.status() != WL_CONNECTED)
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print('.');
  }

  Serial.println("\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Dados Conexão");
  WiFi.printDiag(Serial);

  unsigned int faceID = 1;
  unsigned long frequencia = 44000L;
  String audio = "13b,13a,139,138,136,135,136,13a,138,13b,13d,13d,13c,13d,13e,13d,13d,13e,13f,13e,13e,13e,13f,13e,13d,13d,13c,13c,13b,13b,13b,13b,13b,13c,13c,13d,13d,13f,13e,13f,13f,13f,13f,13f,13f,13f,13f,13f,13f,13f,13e,13d,13c,13c,13b,13b,13b,13a,139,139,139,138,138,136,136,135,135,136,138,138,139,139,13a,139,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13a,139,13a,13a,13a,13a,13b,13c,13c,13d,13d,13d,13d,13d,13d,13c,13c,13c,13c,13b,13b,13b,13a,139,139,139,139,139,138,138,136,135,136,135,134,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,134,134,135,134,134,135,135,135,135,135,136,136,136,136,138,138,138,138,136,138,136,136,138,138,138,138,138,138,138,136,136,136,136,136,136,135,135,135,135,135,135,135,135,135,135,135,136,136,136,136,136,136,138,138,136,138,138,139,138,139,139,139,139,139,139,139,139,139,138,138,138,138,136,136,136,136,136,136,136,136,136,136,136,136,138,138,138,139,139,139,13a,13a,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13a,13a,139,139,138,138,138,138,138,136,138,138,136,136,136,136,136,136,135,136,136,136,138,138,139,13a,13b,13b,13b,13b,13c,13c,13b,13c,13b,13b,13b,13b,139,13a,13a,139,13a,13a,13b,13b,13a,13b,13b,13a,13b,13b,13b,13a,13a,13a,13a,139,139,139,136,138,136,135,136,136,136,135,136,136,136,138,138,139,139,13a,13b,13b,13c,13c,13c,13c,13d,13e,13f,13f,13f,13f,13f,13f,13e,13d,13d,13c,13c,13c,13b,13c,13b,13b,13a,13a,13a,13a,13b,13a,13b,13a,13a,13a,13a,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13e,13d,13d,13d,13d,13c,13c,13c,13b,13b,13b,13c,13b,13b,13a,13b,13b,13b,13b,13b,13c,13b,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13d,13d,13e,13e,13d,13d,13e,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13c,13c,13c,13c,13c,13d,13d,13e,13d,13d,13d,13d,13d,13d,13c,13d,13d,13c,13d,13d,13d,13d,13d,13d,13d,13d,13d,13c,13c,13c,13b,13b,13b,13b,139,139,139,139,139,13a,13a,13b,13a,13a,13b,13b,13b,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13a,13b,13a,139,13a,13a,13a,13a,13a,13b,13a,13a,13b,13b,13b,13b,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13d,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13d,13d,13d,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13b,13a,13b,13b,13b,13c,13b,13c,13c,13b,13b,13b,13a,13b,13a,13b,13b,13b,13c,13b,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13c,13c,13d,13d,13d,13c,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13e,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13a,13b,13b,13b,13b,13c,13c,13c,13b,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13d,13c,13c,13c,13b,13c,13b,13c,13b,13b,13c,13c,13c,13b,13b,13c,13b,13c,13c,13c,13c,13c,13c,13c,13d,13d,13c,13d,13d,13c,13c,13c,13c,13d,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13c,13b,13c,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13b,13c,13b,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13d,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13d,13b,13c,13b,13c,13b,13b,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13b,13c,13c,13c,13b,13b,13c,13b,13c,13c,13b,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13b,13c,13c,13c,13b,13b,13c,13c,13c,13c,13b,13b,13c,13b,13b,13b,13b,13b,13b,13c,13b,13b,13b,13c,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13b,13c,13c,13c,13c,13b,13b,13b,13b,13b,13c,13b,13c,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13b,13c,13c,13b,13b,13b,13b,13b,13c,13b,13c,13c,13c,13c,13c,13b,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13b,13b,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13d,13e,13d,13c,13c,13c,13a,138,13b,13d,13e,13f,140,13d,13e,13a,13b,138,135,13d,138,13c,146,13f,13e,";
  jsonFull = makeJSONAudio(faceID, audio, frequencia);
  Serial.println(jsonFull);

  sendJSON(jsonFull);
}

void loop() {
  delay(15000);
  sendJSON(jsonFull);
}



