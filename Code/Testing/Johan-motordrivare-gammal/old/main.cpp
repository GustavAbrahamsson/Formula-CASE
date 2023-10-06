#include <Arduino.h>

int buttonPin0 = 0;
int buttonPin1 = 1;
bool oldButtonValue0 = HIGH;
bool oldButtonValue1 = HIGH;



int state = 0;



int fwrPin1 = 4;
int revPin1 = 5;

int fwrPin2 = 6;
int revPin2 = 7;

int speed = 0;

int dutyCycle = 20;


// Var for last button press
unsigned long lastButtonPress = 0;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(buttonPin0, INPUT_PULLUP);
    pinMode(buttonPin1, INPUT_PULLUP);

    pinMode(fwrPin1, OUTPUT);
    pinMode(revPin1, OUTPUT);
    pinMode(fwrPin2, OUTPUT);
    pinMode(revPin2, OUTPUT);
    analogWriteFrequency(26000);
    

}

void loop() {
    // // put your main code here, to run repeatedly:

    // Button
    bool buttonValue0 = digitalRead(buttonPin0);
    bool buttonValue1 = digitalRead(buttonPin1);

    if (buttonValue0 == LOW && oldButtonValue0 == HIGH) {
        state += 1;
        if (state > 1) {
            state = 1;
            dutyCycle += 10;
        }
    }
    if (buttonValue1 == LOW && oldButtonValue1 == HIGH) {
        state -= 1;
        if (state < -1) {
            state = -1;
            dutyCycle -= 10;
        }
    }

    switch (state)
    {
    case 1:
        // Forward
        analogWrite(fwrPin1, dutyCycle);
        analogWrite(revPin1, 0);
        state = 0;
        break;
    case -1:
        // Reverse
        analogWrite(fwrPin1, 0);
        analogWrite(revPin1, dutyCycle);
        state = 0;
        break;
    case 0:
        break;

    default:
        break;
    }

    oldButtonValue0 = buttonValue0;
    oldButtonValue1 = buttonValue1;
    Serial.println(dutyCycle);
    delay(10);

}
