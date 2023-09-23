
// Testing with ESP32-S2-DevkitC-1:
//  * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil

// Perhaps of use for S3:
//  * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <string>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *screen = new Arduino_ILI9341(bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */

uint16_t gear_text_size = 15;
uint16_t gear_indicator_width = gear_text_size*6 + 4;
uint16_t gear_indicator_pos_y = 30;
uint16_t gear_text_pos_y = gear_indicator_pos_y + 5;

#define RPM_GAUGE_HEIGHT 20
float old_rpm_float = 0.0;

void draw_rpm_gauge(float percentage){
   
   uint16_t gauge_pos = percentage * SCREEN_WIDTH;
   uint16_t old_gauge_pos = old_rpm_float * SCREEN_WIDTH;

   if(percentage < old_rpm_float){
      // "Shorten" the rpm length to the new by drawing over with black
      screen->fillRect(gauge_pos, 0, old_gauge_pos - gauge_pos, RPM_GAUGE_HEIGHT, 0x0000);
      old_rpm_float = percentage; 
      return;
   }

   old_rpm_float = percentage;

   std::string end_at = "";

   uint16_t green_end = SCREEN_WIDTH / 3;
   uint16_t red_end = SCREEN_WIDTH * 2 / 3;
   uint16_t blue_end = SCREEN_WIDTH;

   uint16_t green_start = 0;
   uint16_t red_start = green_end + 1;
   uint16_t blue_start = red_end + 1;


   if(gauge_pos > red_end){
      blue_end = gauge_pos;
   }
   else if (gauge_pos > green_end){
      red_end = gauge_pos;
      end_at = "red";
   }
   else if (gauge_pos <= green_end){
      green_end = gauge_pos;
      end_at = "green";
   }


   if(old_gauge_pos > red_end){
      blue_start = old_gauge_pos;
   }
   else if (old_gauge_pos > green_end){
      red_start = old_gauge_pos;
   }
   else if (old_gauge_pos <= green_end){
      green_start = old_gauge_pos;
      screen->fillRect(green_start, 0, green_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x07E1);
      if (end_at == "green") return;
   }
   
   

   if (old_gauge_pos < red_end) screen->fillRect(red_start, 0, red_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0xF800);
   if (end_at == "red") {
      return;
   }
   
   screen->fillRect(blue_start, 0, blue_end - old_gauge_pos, RPM_GAUGE_HEIGHT, 0x04FF);

}

void remove_text(std::string text){
   screen->setTextColor(0x0000);
   screen->print(text.c_str());

   // Assume white text
   screen->setTextColor(0xFFFF);
}

void setup(void)
{
   delay(1000);
	#ifdef GFX_EXTRA_PRE_INIT
	GFX_EXTRA_PRE_INIT();
	#endif

	// Init Display
	if (!screen->begin())
	{
	Serial.println("screen->begin() failed!");
	}
	screen->fillScreen(BLACK);

	#ifdef GFX_BL
	pinMode(GFX_BL, OUTPUT);
	digitalWrite(GFX_BL, HIGH);
	#endif

   // Landscape
   screen->setRotation(1);

   screen->drawRect((SCREEN_WIDTH - gear_indicator_width)/2, gear_indicator_pos_y, gear_indicator_width, gear_text_size*8, 0xFFFF);

   screen->setTextSize(gear_text_size);
   screen->setCursor((SCREEN_WIDTH - gear_text_size*5)/2, gear_indicator_pos_y + 3);

}

void loop()
{
	// screen->fillScreen(BLACK);

   for(int i = 1; i <= 8; i++){
      screen->setTextColor(0xFFFF);
      screen->setCursor((SCREEN_WIDTH - gear_text_size*5)/2 + 2, gear_text_pos_y);
      screen->print(i);

      for(double j = 10; j <= 50; j++){
         draw_rpm_gauge(j/50);
         delay(10*i);
      }
      screen->setCursor((SCREEN_WIDTH - gear_text_size*5)/2 + 2, gear_text_pos_y);
      screen->setTextColor(0x0000);
      screen->print(i);

      //remove_text(std::to_string(i));
   }

  
	// gfx->setCursor(random(gfx->width()), random(gfx->height()));
	// gfx->setTextColor(random(0xffff), random(0xffff));
	// gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
	// gfx->println("Hello World!");

	delay(1000); // 1 second
}