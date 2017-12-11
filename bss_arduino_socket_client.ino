
#include <WebSocketsClient.h>
#include <Hash.h>

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h"

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define ON 0 
#define OFF 1
const char* ssid = "anonymous357";
const char* password = "_n@1234567980";

ESP8266WebServer server(80);

SSD1306  display(0x3c, 4, 5);

WebSocketsClient webSocket;

const int led = 16;

void handleRoot() {
  digitalWrite(led, OFF);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, ON);
}

void handleNotFound(){
  digitalWrite(led, OFF);
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
  digitalWrite(led, ON);
}

void drawText(String text){
  display.clear();
  display.drawStringMaxWidth(0, 0, 128, text);
  display.display();
}

void drawString(int16_t x, int16_t y, String text){
  display.drawString(x, y, text);
  display.display();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      Serial.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT("Connected");
    }
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
  }

}
void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  display.clear();
  drawString(0, 0, "Beesight Soft");
  
  pinMode(led, OUTPUT);
  
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, !digitalRead(led));
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Display info on LCD
  drawString(0, 10, String("SSID: " + String(ssid)));
  drawString(0, 20, String("IP: " + WiFi.localIP().toString()));
  
  if (MDNS.begin("esp8266")) {
    drawString(0, 30, "MDNS responder started");
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  digitalWrite(led, ON);
  drawString(0, 40, "HTTP server started");
  Serial.println("HTTP server started");


  // server address, port and URL
  webSocket.begin("192.168.0.101", 3000, "/");
  // event handler
  webSocket.onEvent(webSocketEvent);
  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  
}

void loop() {

  server.handleClient();
  
  webSocket.loop();
  delay(10);
}
