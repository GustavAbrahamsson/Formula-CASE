// Pinmappings
// Motor control pins
#define M1_FWR_PIN 35
#define M1_REV_PIN 36
#define M2_FWR_PIN 37
#define M2_REV_PIN 38
#define M3_FWR_PIN 39
#define M3_REV_PIN 40
#define M4_FWR_PIN 41
#define M4_REV_PIN 42

// ADC pins
#define ADC1_4_PIN 5
#define ADC1_5_PIN 6
#define ADC1_6_PIN 7
#define ADC1_7_PIN 8
#define BAT_SENSE_PIN 9

// Motor sense pins ADC
#define M4_SENSE_PIN 4
#define M3_SENSE_PIN 3
#define M2_SENSE_PIN 2
#define M1_SENSE_PIN 1

// Servo pins
#define SERVO1_PIN 15

typedef struct struct_message {
  // Steering data
    int16_t angle;
    int16_t throttle;
    int16_t brake;
    // byte buttons;
} struct_message;