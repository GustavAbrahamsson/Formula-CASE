
#include <Arduino.h>
#include <TFT_eSPI.h> // For the THT-display
#include <Wire.h>
#include <SPI.h>

#include "TLC_LED_Array.h"

// Pins
#define LED_RESET GPIO_NUM_42
#define LED_DRIVER_ADDR 0x60 // ?

// 15-LED array object
TLC_LED_Array tlc(LED_DRIVER_ADDR, LED_RESET);
// All 15 LEDs at max brightness: 87 mA @ 4.03 V

// Important: Setup file 'Setup70b_ESP32_S3_ILI9341.h' is changed to correspond to the used pins
TFT_eSPI tft_display = TFT_eSPI(); 

#define SCREEN_WIDTH_FC 320
#define SCREEN_HEIGHT_FC 240

#define GEAR_FONT 6
#define GEAR_FONT_SIZE 5

const uint16_t gear_text_size = 15;
const uint16_t gear_indicator_width = gear_text_size * 6 + 4;
const uint16_t gear_indicator_pos_y = 30;
const int16_t gear_text_x_shift = 1;
const uint16_t gear_text_pos_y = gear_indicator_pos_y + 5;

#define RPM_GAUGE_HEIGHT 20
float old_rpm_float = 0.0;

void draw_rpm_gauge(float percentage)
{

  uint16_t gauge_pos = percentage * SCREEN_WIDTH_FC;
  uint16_t old_gauge_pos = old_rpm_float * SCREEN_WIDTH_FC;

  if (percentage < old_rpm_float)
  {
    // "Shorten" the rpm length to the new by drawing over with black
    tft_display.fillRect(gauge_pos, 0, old_gauge_pos - gauge_pos, RPM_GAUGE_HEIGHT, 0x0000);
    old_rpm_float = percentage;
    return;
  }

  old_rpm_float = percentage;

  std::string end_at = "";

  uint16_t green_end = SCREEN_WIDTH_FC / 3;
  uint16_t red_end = SCREEN_WIDTH_FC * 2 / 3;
  uint16_t blue_end = SCREEN_WIDTH_FC;

  uint16_t green_start = 0;
  uint16_t red_start = green_end + 1;
  uint16_t blue_start = red_end + 1;

  if (gauge_pos > red_end)
  {
    blue_end = gauge_pos;
  }
  else if (gauge_pos > green_end)
  {
    red_end = gauge_pos;
    end_at = "red";
  }
  else if (gauge_pos <= green_end)
  {
    green_end = gauge_pos;
    end_at = "green";
  }

  if (old_gauge_pos > red_end)
  {
    blue_start = old_gauge_pos;
  }
  else if (old_gauge_pos > green_end)
  {
    red_start = old_gauge_pos;
  }
  else if (old_gauge_pos <= green_end)
  {
    green_start = old_gauge_pos;
    tft_display.fillRect(green_start, 0, green_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x07E1);
    if (end_at == "green")
      return;
  }

  if (old_gauge_pos < red_end)
    tft_display.fillRect(red_start, 0, red_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0xF800);
  if (end_at == "red")
  {
    return;
  }

  tft_display.fillRect(blue_start, 0, blue_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x04FF);
}

void remove_text(std::string text)
{
  tft_display.setTextColor(0x0000);
  tft_display.print(text.c_str());

  // Assume white text
  tft_display.setTextColor(0xFFFF);
}


void setup(void)
{
  delay(1000);

  Serial.begin(115200);
  //Serial.println("Program started");

  delay(100);

  //Serial.println("tlc.begin()");
  tlc.begin();
  
  delay(100);

  //Serial.println("tft_display.begin()");
  tft_display.begin();

  delay(100);

  // Turn off all the LEDs
  //Serial.println("tlc.reset_LEDs()");
  tlc.reset_LEDs();

  delay(500);

  for(int j = 0; j < 2; j++){
    // "Slide"
    for(int i = 0; i < TLC_NUM_LEDS; i++){
      tlc.set_LED(i, 100);
      delay(25);
      tlc.set_LED(i, 0);
    }
  }
  
  // Landscape
  tft_display.setRotation(1);
  tft_display.fillScreen(TFT_BLACK);

  tft_display.drawRect((SCREEN_WIDTH_FC - gear_indicator_width) / 2, gear_indicator_pos_y, gear_indicator_width, gear_text_size * 8, 0xFFFF);
  tft_display.setTextSize(GEAR_FONT_SIZE);
  tft_display.setTextFont(4);
  tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2, gear_indicator_pos_y + 3);
  
}

void loop()
{
  Serial.println("loop()");

  tlc.ramp_set(0);
  delay(500);
  tlc.ramp_set(10);
  delay(500);
  tlc.ramp_set(20);
  delay(500);
  tlc.ramp_set(40);
  delay(500);
  tlc.ramp_set(60);
  delay(500);
  tlc.ramp_set(100);
  delay(500);
  tlc.ramp_set(100);
  delay(500);
  tlc.ramp_set(150);
  delay(500);
  tlc.ramp_set(200);
  delay(500);
  tlc.ramp_set(250);
  delay(500);
  tlc.ramp_set(255);
  delay(500);
  tlc.ramp_set(250);
  delay(500);
  tlc.ramp_set(200);
  delay(500);
  tlc.ramp_set(150);
  delay(500);
  tlc.ramp_set(100);
  delay(500);
  tlc.ramp_set(80);
  delay(500);
  tlc.ramp_set(60);
  delay(500);
  tlc.ramp_set(40);
  delay(500);
  tlc.ramp_set(20);
  delay(500);



  // // Slide
  // for(int i = 0; i < TLC_NUM_LEDS; i++){
  //   tlc.set_LED(i, 127);
  //   delay(25);
  //   tlc.set_LED(i, 0);
  // }
  // delay(250);

  // // Ramp all
  // for(int j = 0; j < 256; j++){
  //   for(int i = 0; i < TLC_NUM_LEDS; i++){
  //     tlc.set_LED(i, j);
  //   }
  //   delay(2);
  // }
  // delay(250);

  
  // // Ramp all
  // for(int j = 0; j < 256; j++){
  //   for(int i = 0; i < TLC_NUM_LEDS; i++){
  //     tlc.set_LED(i, 255 - j);
  //   }
  //   delay(1);
  // }
  // // Ramp all
  // for(int j = 0; j < 256; j++){
  //   for(int i = 0; i < TLC_NUM_LEDS; i++){
  //     tlc.set_LED(i, j);
  //   }
  //   delay(1);
  // }
  

  
  // delay(1000);
  // for(int i = 0; i < TLC_NUM_LEDS; i++){
  //   tlc.set_LED(i, 0);
  //   delay(50);
  // }
  // delay(250);
  // for(int i = 0; i < TLC_NUM_LEDS; i++){
  //   tlc.set_LED(i, 127);
  //   delay(50);
  // }

  tlc.reset_LEDs();


  for (int i = 1; i <= 8; i++)
  {
    tft_display.setTextColor(0xFFFF);
    tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
    tft_display.setTextSize(GEAR_FONT_SIZE);
    tft_display.print(i);

    for (double j = 10; j <= 50; j++)
    {
      draw_rpm_gauge(j / 50);
      
      tlc.ramp_set(255.0 * j/50);

      delay(10 * i);
    }

    tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
    tft_display.setTextColor(0x0000);
    tft_display.setTextSize(GEAR_FONT_SIZE);
    tft_display.print(i);
  }

  delay(250);
}

// ---------------------------------------------

// #include <Arduino.h>
// #include <TFT_eSPI.h>
// #include <SPI.h>

// TFT_eSPI tft_display = TFT_eSPI(); 

// void setup() {
//   delay(1000);
//   Serial.begin(115200);
//   Serial.println("Program started");
  
//   delay(1000);

//   tft_display.begin();
  
//   delay(1000);

//   tft_display.setRotation(1);
//   tft_display.fillScreen(TFT_BLACK);
//   delay(3000);

// }

// void loop() {
//   Serial.println("loop()");
  
  
//   //Serial.println("SS = CS: " + String(SS));
//   Serial.println("TFT_RST: " + String(TFT_RST));
//   Serial.println("TFT_DC: " + String(TFT_DC));
//   Serial.println("MOSI: " + String(MOSI));
//   Serial.println("SCK: " + String(SCK));
//   Serial.println("(MISO: " + String(MISO) + ")\n");

//   tft_display.fillScreen(TFT_GREEN);
//   delay(3000);
//   tft_display.fillScreen(TFT_BLUE);
//   delay(3000);
//   tft_display.fillScreen(TFT_RED);
//   delay(3000);
  
// }







// ------------------------------------------
// /*
// Scanning...
// I2C device found at address 0x60  !
// I2C device found at address 0x68  !
// I2C device found at address 0x6B  !
// done*/


// // I2C address scanner program
// #include <Wire.h>
// #include <Arduino.h>

// void setup()
// {
//   Wire.begin();
//   Serial.begin(115200);
//   Serial.println("I2C Scanner");

  
//   // Temp for LED circuit
//   pinMode(GPIO_NUM_42, OUTPUT);
//   digitalWrite(GPIO_NUM_42, 1);
// }

// void loop()
// {
//   byte error, address;
//   int nDevices;

//   Serial.println("Scanning...");

//   nDevices = 0;
//   for(address = 1; address < 127; address++ )
//   {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address < 16)
//         Serial.print("0");

//       Serial.print(address,HEX);
//       Serial.println("  !");

//       nDevices++;
//     }
//     else if (error==4)
//     {
//       Serial.print("Unknown error at address 0x");
//       if (address < 16)
//         Serial.print("0");

//       Serial.println(address,HEX);
//     }
//   }

//   if (nDevices == 0)
//     Serial.println("No I2C devices found");
//   else
//     Serial.println("done");

//   delay(5000); // wait 5 seconds for next scan
// }