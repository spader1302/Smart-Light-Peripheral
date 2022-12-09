#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <string>

#include <Light.hpp>
#include <WiFiCredentials.hpp>

ESP8266WebServer server(80);
static const IPAddress LOCAL_IP(192, 168, 0, 5);
static const IPAddress GATEWAY_IP(192, 168, 0, 1);
static const IPAddress SUBNETMASK(255, 255, 255, 0);

static constexpr unsigned DESK_LIGHT_PIN = 14;
static constexpr unsigned WORK_LIGHT_PIN = 16;
static constexpr unsigned MISC1_LIGHT_PIN = 12;
static constexpr unsigned MISC2_LIGHT_PIN = 13;

Light desklight(DESK_LIGHT_PIN);
Light worklight(WORK_LIGHT_PIN);
Light light3(MISC1_LIGHT_PIN);
Light light4(MISC2_LIGHT_PIN);

void invalidRequest() 
{
  server.send(400, "text/plain", "400: Invalid Request");
}

void sendLightState(Light * light)
{
  String jsonString = "{\"state\":\"";
  jsonString += light->getOnState()?"on":"off";
  jsonString += "\",\"pwmValue\":\"";
  jsonString += light->getPWM();
  jsonString += "\"}";

  server.send(200, "application/json", jsonString);
}

void handleLightToggle(Light * light)
{  
  if ( ! server.hasArg("state")) {
    invalidRequest();
    return;
  }  

  if (server.arg("state") == "on") {
    light->on();
  }
  else if (server.arg("state").equals("off")) {
    light->off();
  }

  if (server.hasArg("pwmValue")) {    
    light->pwm(server.arg("pwmValue").toInt());
  }
  sendLightState(light);  
}

void handleRoot() {
 server.send(200, "text/html", "<a href=/desklight>Desk Light</a><br><a href=/worklight>Work Light</a>");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {

  pinMode(DESK_LIGHT_PIN, OUTPUT);
  pinMode(WORK_LIGHT_PIN, OUTPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.config(LOCAL_IP, GATEWAY_IP, SUBNETMASK);
  WiFi.begin(WIFI_CREDENTIALS::SSID, WIFI_CREDENTIALS::PASSPHRASE);

  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_CREDENTIALS::SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/desklight", HTTP_GET, [](){
    sendLightState(&desklight);  
  });

  server.on("/worklight", HTTP_GET, [](){
    sendLightState(&worklight);  
  });

  server.on("/light3", HTTP_GET, [](){
    sendLightState(&light3);  
  });

  server.on("/light4", HTTP_GET, [](){
    sendLightState(&light4);  
  });

  server.on("/desklight", HTTP_POST, [](){
    handleLightToggle(&desklight);  
  });

  server.on("/worklight", HTTP_POST, [](){
    handleLightToggle(&worklight);   
  });  

  server.on("/light3", HTTP_POST, [](){
    handleLightToggle(&light3);   
  });

  server.on("/light4", HTTP_POST, [](){
    handleLightToggle(&light4);   
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}