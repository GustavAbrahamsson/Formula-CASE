
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Main library used
#include <numeric>
#include <vector>

// RPM LED strip
#include "TLC_LED_Array.h"

// For the TFT SPI 240*320 display
#include "FC_Display.h"

// Global variables and definitions
#include "Global_Variables.h"

// Functions for all the peripherals
#include "Peripheral_Functions.h"

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

int16_t low_pass_filter(int16_t new_val, int16_t old_val, float alpha){
  return alpha * new_val + (1 - alpha) * old_val;
}

float read_battery_voltage(){
  return battery_voltage_coeff * analogRead(BAT_LEVEL_PIN);
}

/* Filter the raw data from digital input
@param input Address to an 'Input_Peripheral'
*/
template <typename Func>
void debounce_input(Peripheral_Digital<Func>* input){
  // New value detected, init debouncing
    if((input->raw_val != input->current_val) && !input->debounce_state){
      Serial.println("Debounce started, old: " + String(input->current_val) + 
              " new: " + String(input->raw_val));
      input->current_val = input->raw_val; // Immediately update the value in use

      // Call the assigned function to that specific input and send in the current state
      input->assigned_function(input->current_val);

      input->debounce_state = true;
      input->input_streak = 1;
    }else if(input->debounce_state){
      // If enough samples are identical in a row, assume bouncing has settled
      if (input->input_streak >= PADDLE_DEBOUNCE_STREAK){
        Serial.println("Debounce ended, new: "+String(input->raw_val)+
              ", num_same = "+String(input->input_streak)+"\n\n");
        input->debounce_state = false;
        input->input_streak = 0;
      }
      // Count streak of the same value during debouncing
      else if (input->raw_val == input->last_val) {
        input->input_streak++;
      }
      else {
        input->input_streak = 1;
      }
      Serial.println("Num same: " + String(input->input_streak));
    }

    input->last_val = input->raw_val;
}

// The task that handles reading digital inputs
void button_task(void *pvParameter){

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000.0 / BUTTON_TASK_FREQ);
  BaseType_t xWasDelayed;
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();
  bool debounce_paddle_R = false;
  bool last_paddle_R = false;
  uint16_t num_same_paddle_R = 1;

  bool debounce_paddle_L = false;
  uint16_t num_same_paddle_L = 1;

  while(1){
    // Wait for the next cycle.
    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    // xWasDelayed value can be used to determine whether a deadline was missed
    // if the code here took too long.
    
    Paddle_R.raw_val = digitalRead(R_PADDLE_PIN);
    Paddle_L.raw_val = digitalRead(L_PADDLE_PIN);

    disp.battery_SoC(read_battery_voltage());

    //disp.throttle(throttle_trigger.current_val);

    debounce_input(&Paddle_R);
    debounce_input(&Paddle_L);
  }
}

// The task that handles reading analog inputs
void trigger_task(void *pvParameter){
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000.0 / TRIGGER_TASK_FREQ);
  BaseType_t xWasDelayed;
  xLastWakeTime = xTaskGetTickCount();
  
  uint16_t temp_val;

  while(1){
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);

    // Update old values
    throttle_trigger.old_val = throttle_trigger.current_val;
    brake_trigger.old_val = throttle_trigger.current_val;

    // Update with measurements
    throttle_trigger.measurement = analogRead(R_TRIGGER_PIN);
    brake_trigger.measurement = analogRead(L_TRIGGER_PIN);

    // Filter the input
    throttle_trigger.current_val = low_pass_filter(throttle_trigger.measurement, throttle_trigger.old_val, TRIG_LP_ALPHA);
    brake_trigger.current_val = low_pass_filter(brake_trigger.measurement, brake_trigger.old_val, TRIG_LP_ALPHA);

    // if(brake_trigger.measurement > throttle_trigger.current_val){
    //   temp_val = brake_trigger.measurement;
    // }
    // else{
    //   temp_val = throttle_trigger.current_val;
    // }

    // tlc.ramp_set(255.0 * (float)(temp_val-975)/1400.0);
    
  }
}


void init_tasks(){
  xTaskCreatePinnedToCore(button_task, "button_task", 2048, NULL, 1, NULL, BUTTON_TASK_CORE);
  
  xTaskCreatePinnedToCore(trigger_task, "trigger_task", 2048, NULL, 1, NULL, TRIGGER_TASK_CORE);

}

void setup(void)
{
  delay(1000);
  setup_pins();
  beep_beep();
  Serial.begin(115200);
  Serial.println("Program started");
  delay(100);
  Serial.println("tlc.begin()");
  tlc.begin();
  delay(100);

  Serial.println("disp.begin()");
  disp.begin();
  delay(100);

  // Turn off all the LEDs
  //Serial.println("tlc.reset_LEDs()");
  tlc.reset_LEDs();

  //for(int j = 0; j < 2; j++){
    // "Slide"
  for(int i = 0; i < TLC_NUM_LEDS; i++){
    tlc.set_LED(i, 100);
    delay(10);
    tlc.set_LED(i, 0);
  }
  //}

  tlc.reset_LEDs();
  tlc.ramp_init(0);
  disp.init_gear();

  init_tasks();
}


void loop()
{
  while(1){
    Serial.println("Throttle: "+String(throttle_trigger.current_val));
    Serial.println("Brake: "+String(brake_trigger.measurement));
    Serial.println();
    vTaskDelay(20 / portTICK_RATE_MS);
  }
  

  // tft_display.setCursor(SCREEN_WIDTH_FC / 2 + 60, 25);
  // tft_display.setTextSize(1);
  throttle_R = analogRead(R_TRIGGER_PIN);
  btn1_state = digitalRead(BTN1_PIN);
  // tft_display.println(throttle_R);
  pitch = 1500*((float)(throttle_R-975) / 2300.0) + 500;
  
  // tft_display.setCursor(SCREEN_WIDTH_FC / 2 + 60, 50);
  // tft_display.println(pitch);
  if (btn1_state) (buzz1(pitch));
  else (buzz1(0));




  // Serial.println("Analog: " + String(throttle_R));
  // Serial.println("Pitch: " + String(pitch));
  // Serial.println("BTN1: " + String(btn1_state));
  // Serial.println();

  delay(20);

}

// double mu;
//   double sq_sum;
//   double sigma;
//   double smallest;
//   double highest;
//   int size = 5000;
//   std::vector<double> value_vector(size);
//   while(1){
//     smallest = 5000;
//     highest = 0;
//     for(int i = 0; i < size; i++){
//       value_vector.at(i) = throttle_trigger.measurement;
//       if (value_vector.at(i) < smallest){
//         smallest = value_vector.at(i);
//       }
//       else if(value_vector.at(i) > highest){
//         highest = value_vector.at(i);
//       }
//       vTaskDelay(5 / portTICK_RATE_MS);
//     }
    
//     mu = (double)std::accumulate(value_vector.begin(), value_vector.end(), 0.0) / (double)(size);
    
//     sq_sum = std::inner_product(value_vector.begin(), value_vector.end(), value_vector.begin(), 0.0);
//     sigma = std::sqrt(sq_sum / value_vector.size() - mu * mu);

//     Serial.println("Raw: "+String(throttle_trigger.measurement)+" Mean: "+String(mu)+" 3-sigma: "+String(3*sigma));
//     Serial.println("Highest: "+String(highest)+" Smallest: "+String(smallest)+"\n");
//     Serial.println("Highest: "+String(mu+3*sigma)+" Smallest: "+String(mu-3*sigma)+" (3-sigma +- mu)\n");
//     vTaskDelay(1000 / portTICK_RATE_MS);
//   }

//Serial.println(new_paddle_L);

// if((new_paddle_R != paddle_R) && (millis() - last_paddle_R_time > BUTTON_DEBOUNCE_MS)){
//   paddle_R = new_paddle_R;
//   last_paddle_R_time = millis();
//   Serial.println("Paddle R: " + String(paddle_R));

//   if(paddle_R){
//     current_gear++;
//     disp.change_gear(current_gear);
//     Serial.println("Gear: " + String(current_gear));
//   }
// }

// if((new_paddle_L != paddle_L) && (millis() - last_paddle_L_time > BUTTON_DEBOUNCE_MS)){
//   paddle_L = new_paddle_L;
//   last_paddle_L_time = millis();

//   if(paddle_L){
//     if(current_gear > 0){
//       current_gear--;
//       disp.change_gear(current_gear);
//     }
//   }
// }



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