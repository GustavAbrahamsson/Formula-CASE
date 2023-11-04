#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); 


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println("Program started");

  tft.begin();

  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  delay(3000);

}

void loop() {
  tft.fillScreen(TFT_GREEN);
  delay(3000);
  tft.fillScreen(TFT_BLUE);
  delay(3000);
  tft.fillScreen(TFT_RED);
  delay(3000);
  
}
