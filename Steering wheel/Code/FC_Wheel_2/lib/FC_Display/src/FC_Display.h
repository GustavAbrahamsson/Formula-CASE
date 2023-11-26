// Class definition for the display

#include <TFT_eSPI.h> // Main library used

#define SCREEN_WIDTH_FC 320
#define SCREEN_HEIGHT_FC 240

#define GEAR_FONT 6
#define GEAR_FONT_SIZE 5

#define RPM_GAUGE_HEIGHT 20

#define GEAR_INDEX_MAX 8


class FC_Display{
    private:
        static const uint16_t gear_text_size = 15;
        static const uint16_t gear_indicator_width = gear_text_size * 6 + 4;
        static const uint16_t gear_indicator_pos_y = 30;
        static const int16_t gear_text_x_shift = 1;
        static const uint16_t gear_text_pos_y = gear_indicator_pos_y + 5;
        uint8_t indicated_gear = 0;
        float old_rpm_float = 0.0;

        // Important: Setup file 'Setup70b_ESP32_S3_ILI9341.h' is changed to correspond to the used pins
        // The core instance of the main display library
        TFT_eSPI* tft_display;

        void remove_text(String text);

    public:
        FC_Display(TFT_eSPI* disp_addr);
        void begin();
        void init_gear();
        void change_gear(uint8_t gear);
        void draw_rpm_gauge(float percentage);
        



};

