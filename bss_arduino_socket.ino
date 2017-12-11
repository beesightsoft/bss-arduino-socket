#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <Hash.h>

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h"
#include <WiFiClient.h>
#include <ArduinoJson.h>

#define ON 0 
#define OFF 1
const char* ssid = "anonymous357";
const char* password = "_n@1234567980";

SSD1306  display(0x3c, 4, 5);

WebSocketsServer wss = WebSocketsServer(81);
WebSocketsClient webSocket;

bool socketClientRequest = false;
bool socketClientInit = false;

const int led = 16;

JsonObject& parseJSON(String json) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseObject() failed");
  }
  return root;
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

void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = wss.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
                // send message to client
                wss.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }
}

void webSocketClientEvent(WStype_t type, uint8_t * payload, size_t length) {

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

void setupSocketClient(String host, uint16_t port, String url = "/") {
  if(socketClientRequest) {
    if(!socketClientInit) {
      // server address, port and URL
      webSocket.begin(address, port, url);
      // event handler
      webSocket.onEvent(webSocketClientEvent);
      // try ever 5000 again if connection has failed
      webSocket.setReconnectInterval(5000);
    } else {
      webSocket.loop();  
    }
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

  digitalWrite(led, ON);

  wss.begin();
  wss.onEvent(webSocketServerEvent);

}

void loop() {

  server.handleClient();

  wss.loop();
  

  delay(10);
}
