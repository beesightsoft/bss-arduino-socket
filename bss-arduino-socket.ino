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

WebSocketsServer socketServer = WebSocketsServer(81);
WebSocketsClient socketClient;

bool socketServerStopped = false;
bool socketClientInit = false;

const int led = 16;

void drawText(String text) {
  display.clear();
  display.drawStringMaxWidth(0, 0, 128, text);
  display.display();
}

void drawString(int16_t x, int16_t y, String text) {
  display.drawString(x, y, text);
  display.display();
}

void setupSocketClient(String host, uint16_t port, String path = "/") {
  // server address, port and URL
  socketClient.begin(host, port, path);
  // event handler
  socketClient.onEvent(webSocketClientEvent);
  // try ever 5000 again if connection has failed
  socketClient.setReconnectInterval(5000);

  socketServer.disconnect();
  socketServerStopped = true;
  socketClientInit = true;
}

void socketServerLoop() {
  if(!socketServerStopped) {
    socketServer.loop();    
  }
}

void socketClientLoop() {
  if(socketClientInit) {
    socketClient.loop();  
  }
}

void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  StaticJsonBuffer<200> jsonBuffer;
  switch(type) {
      case WStype_DISCONNECTED:
          Serial.printf("[%u] Disconnected!\n", num);
          break;
      case WStype_CONNECTED:
          {
            IPAddress ip = socketServer.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            drawString(0, 50, String("Client connected"));
          }
          break;
      case WStype_TEXT:
          {
            //String config = "{\"host\":\"192.168.0.101\",\"port\":3000,\"path\":\"/\"}";
            JsonObject& root = jsonBuffer.parseObject((char*) payload);
            if (!root.success()) {
              Serial.println("parseObject() failed");
              break;
            }
            
            const char* host = root["host"];
            int16_t port = root["port"];
            const char* path = root["path"];
  
            setupSocketClient (host, port, path);
            Serial.printf("[%u] get Config: %s:%d%s\n", num, host, port, path);
       
            // send message to client
            socketServer.sendTXT(num, "{\"esp\":\"done\"}");
          }
          break;
  }
}

void webSocketClientEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: 
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        drawString(0, 50, String("Connected to server"));
      }
      break;
    case WStype_TEXT:
      {
        Serial.printf("[WSc] get text: %s\n", payload);

        char * command = (char*) payload;
        
        drawString(0, 40, String(command));
        
        if (strcmp(command, "W") == 0){
            
        } else if (strcmp(command, "A") == 0){
            
        } else if (strcmp(command, "D") == 0){
            
        } else if (strcmp(command, "S") == 0){
            
        }
        
        // send message to server
        // socketClient.sendTXT("message here");
      }
      break;
  }
}

void setup() {
  pinMode(led, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  display.clear();
  drawString(0, 0, "Beesight Soft");
  
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, !digitalRead(led));
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected to: %s\n", ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Display info on LCD
  drawString(0, 10, String("SSID: " + String(ssid)));
  drawString(0, 20, String("IP: " + WiFi.localIP().toString()));

  digitalWrite(led, ON);

  socketServer.begin();
  socketServer.onEvent(webSocketServerEvent);

}

void loop() {
  socketServerLoop();
  
  socketClientLoop();
  
  delay(10);
}
