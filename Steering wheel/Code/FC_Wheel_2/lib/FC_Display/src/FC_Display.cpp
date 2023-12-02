#include <Arduino.h>
#include "FC_Display.h"

FC_Display::FC_Display(TFT_eSPI* disp_addr){
  Serial.println("FC_Display()");
  tft_display = disp_addr;
}

void FC_Display::begin(){
  tft_display->begin();
  delay(100);

  // Init main screen
  init_main();
}

// Boots up the main screen
void FC_Display::init_main(){
  // Landscape
  tft_display->setRotation(1);
  tft_display->fillScreen(TFT_BLACK);

  init_gear();
  init_bar(&throttle_bar, 0);
  init_bar(&brake_bar, 0);
}

// Init a certain status bar
void FC_Display::init_bar(status_bar* bar, uint8_t value){
  // Reset the area
  tft_display->fillRect(bar->x0, bar->y0, bar->width, bar->height, TFT_BLACK);

  // Draw a white rectangle around the bar
  tft_display->drawRect(bar->x0, bar->y0, bar->width, bar->height, TFT_WHITE);
  update_bar(bar, value);
}

// Update a status bar
void FC_Display::update_bar(status_bar* bar, uint8_t value){
  value = constrain(value, 0, 255);
  bar->value = value;
  uint16_t bar_height_total = (bar->height-2*bar_spacing);
  uint16_t old_tip_y = bar_height_total * (float)(255.0 - bar->old_value)/255.0;
  uint16_t new_tip_y = bar_height_total * (float)(255.0 - bar->value)/255.0;

  uint16_t y_start;
  uint16_t diff_height = abs(new_tip_y - old_tip_y);
  int color_in_use;

  if (bar->value > bar->old_value){ // Increase (use specified color)
    y_start = new_tip_y;
    color_in_use = bar->color;
  }
  else if (bar->value < bar->old_value){ // Decrease (remove with black)
    y_start = old_tip_y;;
    color_in_use = TFT_BLACK;
  }
  else{ // No change
    return;
  }

  tft_display->fillRect(bar->x0 + bar_spacing, bar->y0 + bar_spacing + y_start, 
              bar->width - 2 * bar_spacing, diff_height, color_in_use);

  bar->old_value = bar->value;
}

// Initialize the gear indicator
void FC_Display::init_gear(){
  // Initialize gear indicator
  tft_display->drawRect((SCREEN_WIDTH_FC - gear_indicator_width) / 2, gear_indicator_pos_y,
                                            gear_indicator_width, gear_text_size * 8, 0xFFFF);
  change_gear(indicated_gear);
}


/* Change the gear number on the display
  @param gear Which gear index to show, with '0' being neutral [0, GEAR_INDEX_MAX]
*/
void FC_Display::change_gear(uint8_t gear){
  String out;
  tft_display->setTextFont(GEAR_FONT);
  if(indicated_gear == 0) {
    out = "N";
    tft_display->setTextSize(GEAR_FONT_SIZE-1); // Reduce size for 'N', and shift it
    tft_display->setCursor(gear_text_x_shift-2 + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2,
                                                                  gear_indicator_pos_y + 15);
  } else {
    out = String(indicated_gear);
    tft_display->setTextSize(GEAR_FONT_SIZE); // Normal size
  tft_display->setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2,
                                                                  gear_indicator_pos_y + 3);
  }
  remove_text(out);
  uint8_t gear_val = constrain(gear, 0, GEAR_INDEX_MAX);
  indicated_gear = gear_val; // Update local "current_gear"
  if(indicated_gear == 0) {
    out = "N";
    tft_display->setTextSize(GEAR_FONT_SIZE-1); // Reduce size for 'N', and shift it
    tft_display->setCursor(gear_text_x_shift-2 + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2,
                                                                  gear_indicator_pos_y + 15);
  } else {
    out = String(gear_val);
    tft_display->setTextSize(GEAR_FONT_SIZE); // Normal size
    tft_display->setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2,
                                                                  gear_indicator_pos_y + 3);
  }
  tft_display->print(out);
}

/* Display the battery voltage
  @param voltage [V]
*/
void FC_Display::battery_SoC(float voltage){
  tft_display->setTextSize(BAT_FONT_SIZE);
  tft_display->setTextFont(BAT_FONT);
  tft_display->setCursor(bat_pos_x, bat_pos_y);
  remove_text(String(bat_voltage));
  tft_display->setCursor(bat_pos_x, bat_pos_y);
  bat_voltage = voltage;
  tft_display->print(String(bat_voltage) + " V");
}

/* Draw the throttle on the right side
  @param value Range [0, 255]
*/
void FC_Display::throttle(uint8_t value){
  update_bar(&throttle_bar, value);
}

/* Draw the brake on the left side
  @param value Range [0, 255]
*/
void FC_Display::brake(uint8_t value){
  update_bar(&brake_bar, value);
}


void FC_Display::remove_text(String text)
{
  tft_display->setTextColor(0x0000);
  tft_display->print(text.c_str());

  // Assume white text
  tft_display->setTextColor(0xFFFF);
}

/* Draw the angle (z-axis)
  @param angle The angle [deg]
*/
void FC_Display::draw_angle(float angle){

}


void FC_Display::draw_rpm_gauge(float percentage)
{
  uint16_t gauge_pos = percentage * SCREEN_WIDTH_FC;
  uint16_t old_gauge_pos = old_rpm_float * SCREEN_WIDTH_FC;

  if (percentage < old_rpm_float)
  {
    // "Shorten" the rpm length to the new by drawing over with black
    tft_display->fillRect(gauge_pos, 0, old_gauge_pos - gauge_pos, RPM_GAUGE_HEIGHT, 0x0000);
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
    tft_display->fillRect(green_start, 0, green_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x07E1);
    if (end_at == "green")
      return;
  }

  if (old_gauge_pos < red_end)
    tft_display->fillRect(red_start, 0, red_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0xF800);
  if (end_at == "red")
  {
    return;
  }

  tft_display->fillRect(blue_start, 0, blue_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x04FF);
}