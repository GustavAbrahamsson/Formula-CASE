#include <Arduino.h>
#include <TFT_eSPI.h> // For the THT-display
#include <Wire.h>
#include <SPI.h>

#include "TLC_LED_Array.h"

// Pins
#define BAT_LEVEL_PIN GPIO_NUM_1
#define VOLUME_PIN GPIO_NUM_2

#define BTN1_PIN GPIO_NUM_4
#define BTN2_PIN GPIO_NUM_5
#define BTN3_PIN GPIO_NUM_6
#define BTN4_PIN GPIO_NUM_7

#define L_TRIGGER_PIN GPIO_NUM_17
#define R_TRIGGER_PIN GPIO_NUM_18

#define BUZZ_2_PIN GPIO_NUM_41
#define BUZZ_1_PIN GPIO_NUM_40
#define BZR_1_CHANNEL 0
#define BZR_2_CHANNEL 5

#define L_PADDLE_PIN GPIO_NUM_39
#define R_PADDLE_PIN GPIO_NUM_38

#define ENC_B_PIN GPIO_NUM_48
#define ENC_A_PIN GPIO_NUM_47
#define ENC_SW_PIN GPIO_NUM_21

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

void buzz1(uint16_t freq){
   ledcWriteTone(BZR_1_CHANNEL, freq);
}

void buzz2(uint16_t freq){
   ledcWriteTone(BZR_2_CHANNEL, freq);
}

void setup_pins(){
  pinMode(BAT_LEVEL_PIN, INPUT);
  pinMode(VOLUME_PIN, INPUT);

  pinMode(BTN1_PIN, INPUT_PULLDOWN);
  pinMode(BTN2_PIN, INPUT_PULLDOWN);
  pinMode(BTN3_PIN, INPUT_PULLDOWN);
  pinMode(BTN4_PIN, INPUT_PULLDOWN);
  
  pinMode(L_TRIGGER_PIN, INPUT);
  pinMode(R_TRIGGER_PIN, INPUT);

  ledcAttachPin(BUZZ_1_PIN, BZR_1_CHANNEL);
  ledcAttachPin(BUZZ_2_PIN, BZR_2_CHANNEL);

  pinMode(L_PADDLE_PIN, INPUT_PULLDOWN);
  pinMode(R_PADDLE_PIN, INPUT_PULLDOWN);
}

void beep_beep(){
  buzz1(1000);
  delay(100);
  buzz1(0);
  delay(50);
  buzz2(1000);
  delay(100);
  buzz2(0);
}

void setup(void)
{
  delay(1000);

  setup_pins();

  beep_beep();

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

  //for(int j = 0; j < 2; j++){
    // "Slide"
  for(int i = 0; i < TLC_NUM_LEDS; i++){
    tlc.set_LED(i, 100);
    delay(25);
    tlc.set_LED(i, 0);
  }
  //}
  
  // Landscape
  tft_display.setRotation(1);

  tft_display.fillScreen(TFT_BLACK);

  // Gear indicator
  tft_display.drawRect((SCREEN_WIDTH_FC - gear_indicator_width) / 2, gear_indicator_pos_y, gear_indicator_width, gear_text_size * 8, 0xFFFF);
  tft_display.setTextSize(GEAR_FONT_SIZE);
  tft_display.setTextFont(4);
  tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2, gear_indicator_pos_y + 3);
  
  tlc.reset_LEDs();
  
  tlc.ramp_init(0);
}
uint16_t throttle_R;
uint16_t pitch;
bool btn1_state;

uint8_t current_gear = 0;
bool paddle_R = 0;
bool paddle_L = 0;
bool new_paddle_R = 0;
bool new_paddle_L = 0;
uint32_t last_paddle_L_time = 0;
uint32_t last_paddle_R_time = 0;

void loop()
{
  new_paddle_R = digitalRead(R_PADDLE_PIN);
  new_paddle_L = digitalRead(L_PADDLE_PIN);

  Serial.println(new_paddle_L);

  if((new_paddle_R != paddle_R) && (millis() - last_paddle_R_time > 100)){
    paddle_R = new_paddle_R;
    last_paddle_R_time = millis();
    //Serial.println("Paddle R: " + String(paddle_R));

    if(paddle_R){
      current_gear++;
      //Serial.println("Gear: " + String(current_gear));
    }
  }

  if((new_paddle_L != paddle_L) && (millis() - last_paddle_L_time > 100)){
    paddle_L = new_paddle_L;
    last_paddle_L_time = millis();
    Serial.println("Paddle L: " + String(paddle_L));

    if(paddle_L){
      if(current_gear > 0) current_gear--;
      Serial.println("Gear: " + String(current_gear));
    }
  }

  tft_display.setCursor(SCREEN_WIDTH_FC / 2 + 60, 25);
  tft_display.setTextSize(1);
  throttle_R = analogRead(R_TRIGGER_PIN);
  btn1_state = digitalRead(BTN1_PIN);
  tft_display.println(throttle_R);
  pitch = 1500*((float)(throttle_R-975) / 2300.0) + 500;
  
  tft_display.setCursor(SCREEN_WIDTH_FC / 2 + 60, 50);
  tft_display.println(pitch);
  if (btn1_state) (buzz1(pitch));
  else (buzz1(0));

  tlc.ramp_set(255.0 * (float)(throttle_R-975)/1300.0);

  // Gear indicator
  tft_display.drawRect((SCREEN_WIDTH_FC - gear_indicator_width) / 2, gear_indicator_pos_y, gear_indicator_width, gear_text_size * 8, 0xFFFF);
  tft_display.setTextSize(GEAR_FONT_SIZE);
  tft_display.setTextFont(4);
  tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2, gear_indicator_pos_y + 3);
  tft_display.print(current_gear);

  // Serial.println("Analog: " + String(throttle_R));
  // Serial.println("Pitch: " + String(pitch));
  // Serial.println("BTN1: " + String(btn1_state));
  // Serial.println();

  //delay(20);

}

// for (int i = 1; i <= 8; i++)
  // {
  //   tft_display.setTextColor(0xFFFF);
  //   tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
  //   tft_display.setTextSize(GEAR_FONT_SIZE);
  //   tft_display.print(i);

  //   for (double j = 10; j <= 50; j++)
  //   {
  //     draw_rpm_gauge(j / 50);

  //     // if (((int)j % 2) == 0){
  //     //   buzz1(1500 + 500 * j/50);
  //     // }
      
  //     tlc.ramp_set(255.0 * j/50);

  //     delay(10 * i);
  //   }

  //   tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
  //   tft_display.setTextColor(0x0000);
  //   tft_display.setTextSize(GEAR_FONT_SIZE);
  //   tft_display.print(i);
  // }

  // delay(250);




  // for(int k = 0; k < 2; k++){
    
  //   for(int i = 0; i < 255; i++){
  //     tlc.ramp_set(i);
  //     delay(5);
  //   }
  //   for(int i = 0; i < 255; i++){
  //     tlc.ramp_set(255-i);
  //     delay(5);
  //   }

  // }

  
  // tlc.ramp_set(20);
  // delay(1000);

  // tlc.ramp_set(200);
  // delay(1000);
  
  // tlc.ramp_set(100);
  // delay(1000);
  
  // tlc.ramp_set(150);
  // delay(1000);

  
  // tlc.ramp_set(30);
  // delay(1000);
  // tlc.ramp_set(250);
  // delay(1000);



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

//   tlc.reset_LEDs();
//   tlc.ramp_init(0, true);

//   for (int i = 1; i <= 8; i++)
//   {
//     tft_display.setTextColor(0xFFFF);
//     tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
//     tft_display.setTextSize(GEAR_FONT_SIZE);
//     tft_display.print(i);

//     for (double j = 10; j <= 50; j++)
//     {
//       draw_rpm_gauge(j / 50);
      
//       tlc.ramp_set(255.0 * j/50);

//       delay(10 * i);
//     }

//     tft_display.setCursor(gear_text_x_shift + (SCREEN_WIDTH_FC - gear_text_size * 5) / 2 + 2, gear_text_pos_y);
//     tft_display.setTextColor(0x0000);
//     tft_display.setTextSize(GEAR_FONT_SIZE);
//     tft_display.print(i);
//   }

//   delay(250);
// }

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