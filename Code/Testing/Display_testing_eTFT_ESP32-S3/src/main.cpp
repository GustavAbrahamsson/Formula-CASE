

#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

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
  Serial.println("Program started");

  tft_display.begin();
  
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

  for (int i = 1; i <= 8; i++)
  {
    tft_display.setTextColor(0xFFFF);
    tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
    tft_display.setTextSize(GEAR_FONT_SIZE);
    tft_display.print(i);

    for (double j = 10; j <= 50; j++)
    {
      draw_rpm_gauge(j / 50);
      delay(10 * i);
    }
    tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
    tft_display.setTextColor(0x0000);
    tft_display.setTextSize(GEAR_FONT_SIZE);
    tft_display.print(i);
  }

  delay(250);
}


// #include <Arduino.h>
// #include <TFT_eSPI.h> // Hardware-specific library
// #include <SPI.h>

// TFT_eSPI tft_display = TFT_eSPI(); 


// void setup() {
//   delay(1000);
//   Serial.begin(115200);
//   Serial.println("Program started");

//   tft_display.begin();

//   tft_display.setRotation(1);
//   tft_display.fillScreen(TFT_BLACK);
//   delay(3000);

// }

// void loop() {
//   Serial.println("loop()");

//   tft_display.fillScreen(TFT_GREEN);
//   delay(3000);
//   tft_display.fillScreen(TFT_BLUE);
//   delay(3000);
//   tft_display.fillScreen(TFT_RED);
//   delay(3000);
  
// }