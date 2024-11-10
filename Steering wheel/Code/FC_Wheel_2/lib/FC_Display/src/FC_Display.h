// Class definition for the display

#pragma once

#include <TFT_eSPI.h> // Main library used

#define SCREEN_WIDTH_FC 320
#define SCREEN_HEIGHT_FC 240

#define GEAR_FONT 4
#define GEAR_FONT_SIZE 5
#define GEAR_INDEX_MAX 8

#define BAT_FONT 4
#define BAT_FONT_SIZE 1

#define RPM_GAUGE_HEIGHT 20

struct text_box{
  String text;
  uint8_t x_shift;
  uint16_t x0;
  uint16_t y0;
  uint8_t height;
  uint8_t width;
  int color;
};

struct status_bar{
  uint8_t value;
  uint8_t old_value;
  uint16_t x0;
  uint16_t y0;
  uint8_t height;
  uint8_t width;
  int color;
};

class FC_Display{
    private:
        int8_t indicated_gear = 0;
        static const uint16_t gear_text_size = 15;
        static const uint16_t gear_indicator_width = gear_text_size * 6 + 4;
        static const uint16_t gear_indicator_pos_y = 30;
        static const int16_t gear_text_x_shift = 4;
        static const uint16_t gear_text_pos_y = gear_indicator_pos_y + 5;
        
        float old_rpm_float = 0.0;

        float bat_voltage = 0;
        static const uint16_t bat_text_size = 5;
        static const uint16_t bat_pos_x = SCREEN_WIDTH_FC - 75;
        static const uint16_t bat_pos_y = SCREEN_HEIGHT_FC - 25;

        // Throttle bar dimensions
        static const uint16_t bar_distance = 2;
        static const uint16_t bar_spacing = 2;

        static const uint16_t throttle_bar_width = 20;
        static const uint16_t throttle_bar_height = 120;
        static const uint16_t throttle_bar_x0 = SCREEN_WIDTH_FC - throttle_bar_width - bar_distance;
        static const uint16_t throttle_bar_y0 = bar_distance;
        static const uint16_t throttle_color = TFT_GREEN;

        // Brake bar dimensions (mirrored)
        static const uint16_t brake_bar_width = throttle_bar_width;
        static const uint16_t brake_bar_height = throttle_bar_height;
        static const uint16_t brake_bar_x0 = bar_distance;
        static const uint16_t brake_bar_y0 = bar_distance;
        static const uint16_t brake_color = TFT_RED;

        status_bar throttle_bar = {0, 0, throttle_bar_x0, throttle_bar_y0, throttle_bar_height, 
                                          throttle_bar_width, throttle_color};

        status_bar brake_bar =    {0, 0, brake_bar_x0, brake_bar_y0, brake_bar_height,
                                          brake_bar_width, brake_color};

        static const int16_t espnow_box_width = 120;
        static const int16_t espnow_box_height = 20;
        text_box espnow_connected =     {"Connected", 35, SCREEN_WIDTH_FC - espnow_box_width, SCREEN_HEIGHT_FC - espnow_box_height, espnow_box_height, espnow_box_width, TFT_GREEN};
        text_box espnow_no_connection = {"No connection", 25, SCREEN_WIDTH_FC - espnow_box_width, SCREEN_HEIGHT_FC - espnow_box_height, espnow_box_height, espnow_box_width, TFT_RED};


        // Important: Setup file 'Setup70b_ESP32_S3_ILI9341.h' is changed to correspond to the used pins
        // The core instance of the main display library
        TFT_eSPI* tft_display;

        void init_bar(status_bar* bar, uint8_t value);
        void update_bar(status_bar* bar, uint8_t value);

        void remove_text(String text);

        void init_main();

        void init_gear();

    public:
        FC_Display(TFT_eSPI* disp_addr);
        void begin();
        
        void change_gear(int8_t gear);

        void throttle(uint8_t throttle);
        void brake(uint8_t brake);

        void battery_SoC(float voltage);
        void draw_angle(float angle);
        void draw_rpm_gauge(float percentage);
        void draw_text_box(text_box tb);

        void update_comms_status(bool is_connected);
        
};

