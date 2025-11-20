#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> // Needed for the "Notify" feature

#define SERVICE_UUID        "2f49436e-3900-44ea-8837-8843708d9ef0"
#define CHARACTERISTIC_UUID "d2cdedc3-53a3-4a0a-adc7-2b2bf7e757f3"

const int INPUT_PIN_D1 = 3;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// Callback to handle connections/disconnections
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device Connected");
      
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device Disconnected");
      // Restart advertising so we can connect again without rebooting
      BLEDevice::startAdvertising();
    }
};

void setup() {
  Serial.begin(115200);

  // Small delay to allow Serial to initialize before printing
  delay(1000);

  pinMode(INPUT_PIN_D1, INPUT_PULLUP);

  // Create the message
  int currentState = digitalRead(INPUT_PIN_D1);

  // 3. Determine the NEXT wake-up logic
  // If pin is currently HIGH, we want to wake up when it goes LOW.
  // If pin is currently LOW, we want to wake up when it goes HIGH.
  esp_deepsleep_gpio_wake_up_mode_t wakeupMode;

  if (currentState == 1) {
    Serial.println("Configuring wake-up for LOW state...");
    wakeupMode = ESP_GPIO_WAKEUP_GPIO_LOW;
  } else {
    Serial.println("Configuring wake-up for HIGH state...");
    wakeupMode = ESP_GPIO_WAKEUP_GPIO_HIGH;
  }

  // Enable the wake-up source
  // The first argument is a bitmask of the pin ((1ULL << pin_number))
  esp_err_t err = esp_deep_sleep_enable_gpio_wakeup(1ULL << INPUT_PIN_D1, wakeupMode);
  if (err != ESP_OK) {
    Serial.printf("Error enabling GPIO wakeup: %d\n", err);
  }
    Serial.println("blah");

  const char* err_msg = (err != ESP_OK) ? "ERR_SLEEP"  : "OK";

  const char* status = (currentState == 1) ? "HIGH" : "LOW";
  char message[22];

    Serial.println("blah1");
  snprintf(message, sizeof(message), "BLE = %s, %s", status, err_msg);
    Serial.println("blah2");
  
  // Update the value
  // pCharacteristic->setValue(message);
  
  // Send the notification (Push the data to the phone/PC)
  // pCharacteristic->notify();
  
  // Serial.println("Notification sent");
  // go back to sleep
  //   Serial.println("Entering Deep Sleep now...");
  Serial.flush(); // Ensure all serial data is sent before sleeping

  // 5. Start Deep Sleep
  esp_deep_sleep_start();
}

void loop() {
  // Only send data if something is actually connected
  if (deviceConnected) {
  }
}
