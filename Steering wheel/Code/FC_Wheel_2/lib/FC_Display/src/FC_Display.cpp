#include <Arduino.h>
#include "FC_Display.h"

FC_Display::FC_Display(TFT_eSPI* disp_addr){
  Serial.println("FC_Display()");
  FC_Display::tft_display = disp_addr;
}

void FC_Display::begin(){
  
  Serial.println("disp.begin() 1");
  FC_Display::tft_display->begin();
  delay(100);
  // Landscape
  Serial.println("disp.begin() 2");
  FC_Display::tft_display->setRotation(1);

  Serial.println("disp.begin() 3");
  FC_Display::tft_display->fillScreen(TFT_BLACK);
}

// Initialize the gear indicator
void FC_Display::init_gear(){
  // Initialize gear indicator
  FC_Display::tft_display->drawRect((SCREEN_WIDTH_FC - gear_indicator_width) / 2, gear_indicator_pos_y, gear_indicator_width, gear_text_size * 8, 0xFFFF);
  change_gear(indicated_gear);
}


/* Change the gear number on the display
  @param gear Which gear index to show, with '0' being neutral [0, GEAR_INDEX_MAX]
*/
void FC_Display::change_gear(uint8_t gear){
  FC_Display::tft_display->setTextSize(GEAR_FONT_SIZE);
  FC_Display::tft_display->setTextFont(4);
  FC_Display::tft_display->setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2, gear_indicator_pos_y + 3);
  remove_text(String(indicated_gear));
  uint8_t gear_val = constrain(gear, 0, GEAR_INDEX_MAX);
  indicated_gear = gear_val; // Update local "current_gear"
  String out = String(gear_val);
  if(out == 0) out = "N"; // Display "N" as neutral if gear '0' is active
  FC_Display::tft_display->print(out);
}

void FC_Display::remove_text(String text)
{
  FC_Display::tft_display->setTextColor(0x0000);
  FC_Display::tft_display->print(text.c_str());

  // Assume white text
  FC_Display::tft_display->setTextColor(0xFFFF);
}

void FC_Display::draw_rpm_gauge(float percentage)
{
  uint16_t gauge_pos = percentage * SCREEN_WIDTH_FC;
  uint16_t old_gauge_pos = old_rpm_float * SCREEN_WIDTH_FC;

  if (percentage < old_rpm_float)
  {
    // "Shorten" the rpm length to the new by drawing over with black
    FC_Display::tft_display->fillRect(gauge_pos, 0, old_gauge_pos - gauge_pos, RPM_GAUGE_HEIGHT, 0x0000);
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
    FC_Display::tft_display->fillRect(green_start, 0, green_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x07E1);
    if (end_at == "green")
      return;
  }

  if (old_gauge_pos < red_end)
    FC_Display::tft_display->fillRect(red_start, 0, red_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0xF800);
  if (end_at == "red")
  {
    return;
  }

  FC_Display::tft_display->fillRect(blue_start, 0, blue_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x04FF);
}