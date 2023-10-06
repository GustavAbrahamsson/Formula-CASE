// ESP32 c3 MAC: 68:67:25:AD:DD:D4

#include "WiFi.h"
#include "esp_now.h"
#include <Bluepad32.h>


uint8_t broadcastAddress[] = {0x68, 0x67, 0x25, 0xAD, 0xDD, 0xD4};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  // axis data
  int16_t axisX;
  int16_t axisY;
  int16_t axisRX;
  int16_t axisRY;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}



//////////////////////////// Bluepad



GamepadPtr myGamepads[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedGamepad(GamepadPtr gp) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myGamepads[i] == nullptr) {
      Serial.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      GamepadProperties properties = gp->getProperties();
      Serial.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n",
                    gp->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myGamepads[i] = gp;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
        "CALLBACK: Gamepad connected, but could not found empty slot");
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myGamepads[i] == gp) {
      Serial.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
      myGamepads[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
        "CALLBACK: Gamepad disconnected, but not found in myGamepads");
  }
}



void setup(){
  delay(100);
  Serial.begin(115200);
  delay(100);
  Serial.println("Starting");
  WiFi.mode(WIFI_STA);
  Serial.println("MAC: " + WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // pick a channel
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
    

  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2],
                addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();




}

void loop() {
  BP32.update();

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    GamepadPtr myGamepad = myGamepads[i];

    if (myGamepad && myGamepad->isConnected()) {

      // Another way to query the buttons, is by calling buttons(), or
      // miscButtons() which return a bitmask.
      // Some gamepads also have DPAD, axis and more.
      // Serial.printf(
      //     "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: "
      //     "%4d, %4d, brake: %4d, throttle: %4d, misc: 0x%02x, gyro x:%6d y:%6d "
      //     "z:%6d, accel x:%6d y:%6d z:%6d\n",
      //     i,                        // Gamepad Index
      //     myGamepad->dpad(),        // DPAD
      //     myGamepad->buttons(),     // bitmask of pressed buttons
      //     myGamepad->axisX(),       // (-511 - 512) left X Axis
      //     myGamepad->axisY(),       // (-511 - 512) left Y axis
      //     myGamepad->axisRX(),      // (-511 - 512) right X axis
      //     myGamepad->axisRY(),      // (-511 - 512) right Y axis
      //     myGamepad->brake(),       // (0 - 1023): brake button
      //     myGamepad->throttle(),    // (0 - 1023): throttle (AKA gas) button
      //     myGamepad->miscButtons(), // bitmak of pressed "misc" buttons
      //     myGamepad->gyroX(),       // Gyro X
      //     myGamepad->gyroY(),       // Gyro Y
      //     myGamepad->gyroZ(),       // Gyro Z
      //     myGamepad->accelX(),      // Accelerometer X
      //     myGamepad->accelY(),      // Accelerometer Y
      //     myGamepad->accelZ()       // Accelerometer Z
      // );

      // You can query the axis and other properties as well. See Gamepad.h
      // For all the available functions.

      // Set stuct data
      myData.axisX = myGamepad->axisX();
      myData.axisY = myGamepad->axisY();
      myData.axisRX = myGamepad->axisRX();
      myData.axisRY = myGamepad->axisRY();

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
      
      if (result == ESP_OK) {
        // Serial.println("Sent with success");
      }
      else {
        // Serial.println("Error sending the data");
        Serial.println(result);
      }
    }


  }

  



  delay(50);
}
    

