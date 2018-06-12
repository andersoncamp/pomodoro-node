#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;
String jsonFull;

String makeJSONAudio(int faceID, String audio, int frequencia){
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

String makeJSONActivity(int faceID, unsigned long delta){  
  const size_t bufferSize = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer JSONbuffer(bufferSize);
  JsonObject& JSONencoder = JSONbuffer.createObject(); 
  
  JSONencoder["faceID"] = faceID;
  JSONencoder["delta"] = delta;
    
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

void setup()
{
  Serial.begin(115200);
  delay(2000);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("NET_2.4GhzAP12", "617146733");

  Serial.print("Connecting");
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

// EXEMPLO DE ENVIO DE JSON PARA AUDIO COM HTTP POST
//  unsigned int faceID = 1;
//  unsigned long frequencia = 44000L;
//  String audio = "13b,13a,139,138,136,135,136,13a,138,13b,13d,13d,13c,13d,13e,13d,13d,13e,13f,13e,13e,13e,13f,13e,13d,13d,13c,13c,13b,13b,13b,13b,13b,13c,13c,13d,13d,13f,13e,13f,13f,13f,13f,13f,13f,13f,13f,13f,13f,13f,13e,13d,13c,13c,13b,13b,13b,13a,139,139,139,138,138,136,136,135,135,136,138,138,139,139,13a,139,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13a,139,13a,13a,13a,13a,13b,13c,13c,13d,13d,13d,13d,13d,13d,13c,13c,13c,13c,13b,13b,13b,13a,139,139,139,139,139,138,138,136,135,136,135,134,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,134,134,135,134,134,135,135,135,135,135,136,136,136,136,138,138,138,138,136,138,136,136,138,138,138,138,138,138,138,136,136,136,136,136,136,135,135,135,135,135,135,135,135,135,135,135,136,136,136,136,136,136,138,138,136,138,138,139,138,139,139,139,139,139,139,139,139,139,138,138,138,138,136,136,136,136,136,136,136,136,136,136,136,136,138,138,138,139,139,139,13a,13a,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13a,13a,139,139,138,138,138,138,138,136,138,138,136,136,136,136,136,136,135,136,136,136,138,138,139,13a,13b,13b,13b,13b,13c,13c,13b,13c,13b,13b,13b,13b,139,13a,13a,139,13a,13a,13b,13b,13a,13b,13b,13a,13b,13b,13b,13a,13a,13a,13a,139,139,139,136,138,136,135,136,136,136,135,136,136,136,138,138,139,139,13a,13b,13b,13c,13c,13c,13c,13d,13e,13f,13f,13f,13f,13f,13f,13e,13d,13d,13c,13c,13c,13b,13c,13b,13b,13a,13a,13a,13a,13b,13a,13b,13a,13a,13a,13a,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13e,13d,13d,13d,13d,13c,13c,13c,13b,13b,13b,13c,13b,13b,13a,13b,13b,13b,13b,13b,13c,13b,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13d,13d,13e,13e,13d,13d,13e,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13d,13d,13d,13c,13c,13c,13c,13c,13d,13d,13e,13d,13d,13d,13d,13d,13d,13c,13d,13d,13c,13d,13d,13d,13d,13d,13d,13d,13d,13d,13c,13c,13c,13b,13b,13b,13b,139,139,139,139,139,13a,13a,13b,13a,13a,13b,13b,13b,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13a,13b,13a,139,13a,13a,13a,13a,13a,13b,13a,13a,13b,13b,13b,13b,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13d,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13d,13d,13d,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13b,13a,13b,13b,13b,13c,13b,13c,13c,13b,13b,13b,13a,13b,13a,13b,13b,13b,13c,13b,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13c,13c,13d,13d,13d,13c,13d,13d,13d,13d,13d,13d,13d,13d,13d,13d,13e,13d,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13a,13b,13b,13b,13b,13c,13c,13c,13b,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13d,13c,13c,13c,13b,13c,13b,13c,13b,13b,13c,13c,13c,13b,13b,13c,13b,13c,13c,13c,13c,13c,13c,13c,13d,13d,13c,13d,13d,13c,13c,13c,13c,13d,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13c,13b,13c,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13d,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13b,13c,13b,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13d,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13d,13b,13c,13b,13c,13b,13b,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13b,13c,13c,13c,13b,13b,13c,13b,13c,13c,13b,13b,13c,13c,13c,13c,13c,13c,13c,13b,13c,13b,13c,13c,13c,13b,13b,13c,13c,13c,13c,13b,13b,13c,13b,13b,13b,13b,13b,13b,13c,13b,13b,13b,13c,13b,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13b,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13b,13c,13c,13c,13c,13b,13b,13b,13b,13b,13c,13b,13c,13c,13c,13b,13b,13b,13b,13b,13b,13b,13b,13c,13c,13c,13c,13c,13b,13c,13c,13b,13b,13b,13b,13b,13c,13b,13c,13c,13c,13c,13c,13b,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13d,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13b,13c,13c,13c,13c,13c,13b,13b,13c,13c,13c,13b,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13c,13b,13c,13d,13e,13d,13c,13c,13c,13a,138,13b,13d,13e,13f,140,13d,13e,13a,13b,138,135,13d,138,13c,146,13f,13e,";
//  jsonFull = makeJSONAudio(faceID, audio, frequencia);
//  Serial.println(jsonFull);
//  sendJSONAudio(jsonFull);

// EXEMPLO DE ENVIO DE JSON PARA ACTIVITIES COM HTTP POST
//  int faceID = 1;
//  unsigned long delta = 1500;
//  jsonFull = makeJSONActivity(faceID, delta);
//  sendJSONActivity(jsonFull);

// EXEMPLO DE COMO RECUPERAR AS FACES DO POMODORO DE UM USUÁRIO COM HTTP GET
// As faces retornadas são do usuário que está logado
//  JsonArray& json = getFaces();
//  Serial.println(json.size());
//  json.printTo(Serial);
}

void loop() {
}



