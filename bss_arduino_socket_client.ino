#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h"

SSD1306  display(0x3c, 4, 5);

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

}

void drawText(String text){
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128, text);
}

void loop() {
  // clear the display
  display.clear();

  drawText("Hello everybody");
  
  // write the buffer to the display
  display.display();

  delay(10);
}
