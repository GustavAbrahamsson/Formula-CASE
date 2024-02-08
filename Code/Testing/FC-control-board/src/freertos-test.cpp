#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"
#include <Servo.h>
#include "config.h"

#include "freertos/FreeRTOS.h"
#include <semphr.h> // add the FreeRTOS functions for Semaphores (or Flags).

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// define two Tasks for DigitalRead & AnalogRead
void TaskDigitalRead(void *pvParameters);
void TaskAnalogRead(void *pvParameters);



// ESP-NOW
struct_message recivedData;
long lastReciveTime = 0;
long lastPrintTime = 0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&recivedData, incomingData, sizeof(recivedData));
    lastReciveTime = millis();
}

void TaskPrintESPNow(void *pvParameters);


// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200);

    while (!Serial || millis() < 1000)
    {
        ; // wait for serial port to connect.
    }

    // ESP-NOW
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);


    // Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
    // because it is sharing a resource, such as the Serial port.
    // Semaphores should only be used whilst the scheduler is running, but we can set it up here.
    if (xSerialSemaphore == NULL) // Check to confirm that the Serial Semaphore has not already been created.
    {
        xSerialSemaphore = xSemaphoreCreateMutex(); // Create a mutex semaphore we will use to manage the Serial Port
        if ((xSerialSemaphore) != NULL)
            xSemaphoreGive((xSerialSemaphore)); // Make the Serial Port available for use, by "Giving" the Semaphore.
    }

    // Now set up two Tasks to run independently.
    xTaskCreate(
        TaskDigitalRead, "DigitalRead" // A name just for humans
        , 2048 // This stack size can be checked & adjusted by reading the Stack Highwater
        , NULL // Parameters for the task
        , 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        , NULL); // Task Handle

    xTaskCreate(
        TaskAnalogRead, "AnalogRead" // A name just for humans
        , 2048 // Stack size
        , NULL // Parameters for the task
        , 1 // Priority
        , NULL); // Task Handle
    xTaskCreate(
        TaskPrintESPNow, "PrintESPNow" // A name just for humans
        , 2048 // Stack size
        , NULL // Parameters for the task
        , 0 // Priority
        , NULL); // Task Handle
    // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
    // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskDigitalRead(void *pvParameters __attribute__((unused))) // This is a Task.
{
    /*
      DigitalReadSerial
      Reads a digital input on pin 2, prints the result to the serial monitor

      This example code is in the public domain.
    */

    // digital pin 2 has a pushbutton attached to it. Give it a name:
    uint8_t pushButton = 2;

    // make the pushbutton's pin an input:
    pinMode(pushButton, INPUT);

    for (;;) // A Task shall never return or exit.
    {
        // read the input pin:
        int buttonState = digitalRead(pushButton);

        // See if we can obtain or "Take" the Serial Semaphore.
        // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
        {
            // We were able to obtain or "Take" the semaphore and can now access the shared resource.
            // We want to have the Serial Port for us alone, as it takes some time to print,
            // so we don't want it getting stolen during the middle of a conversion.
            // print out the state of the button:
            Serial.print("Button: ");
            Serial.println(buttonState);

            xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
        }

        vTaskDelay(1000); // one tick delay (15ms) in between reads for stability
    }
}

void TaskAnalogRead(void *pvParameters __attribute__((unused))) // This is a Task.
{

    for (;;)
    {
        // read the input on analog pin 0:
        int sensorValue = analogRead(A0);

        // See if we can obtain or "Take" the Serial Semaphore.
        // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
        {
            // We were able to obtain or "Take" the semaphore and can now access the shared resource.
            // We want to have the Serial Port for us alone, as it takes some time to print,
            // so we don't want it getting stolen during the middle of a conversion.
            // print out the value you read:
            Serial.print("Sensor: ");
            Serial.println(sensorValue);

            xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
        }

        vTaskDelay(1000); // one tick delay (15ms) in between reads for stability
    }
}

void TaskPrintESPNow(void *pvParameters)
{
    for (;;)
    {
        if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
        {
            if (lastPrintTime > lastReciveTime) {
                Serial.println("No new data recived");
                vTaskDelay(900);
                xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
            }
            else {
                Serial.println("Recived data: ");
                Serial.print(recivedData.angle);
                Serial.print(" ");
                Serial.print(recivedData.throttle);
                Serial.print(" ");
                Serial.print(recivedData.brake);
                Serial.print(" ");
                Serial.println();
                lastPrintTime = millis();
                xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
                vTaskDelay(900);
            }
        }
        vTaskDelay(100);
    }
}