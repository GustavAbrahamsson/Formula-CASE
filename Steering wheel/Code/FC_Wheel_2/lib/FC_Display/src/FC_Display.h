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



class FC_Display{
    private:
        uint8_t indicated_gear = 0;
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

        // Important: Setup file 'Setup70b_ESP32_S3_ILI9341.h' is changed to correspond to the used pins
        // The core instance of the main display library
        TFT_eSPI* tft_display;

        void remove_text(String text);

    public:
        FC_Display(TFT_eSPI* disp_addr);
        void begin();

        void init_gear();
        void change_gear(uint8_t gear);

        void throttle(uint8_t throttle);
        void brake(uint8_t brake);

        void battery_SoC(float voltage);

        void draw_rpm_gauge(float percentage);
        



};

