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

  pinMode(INPUT_PIN_D1, INPUT_PULLUP);

  // 1. Initialize BLE with a name
  BLEDevice::init("ESP32_BLE_MSG");

  // 2. Create the Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. Create the Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. Create the Characteristic
  // This is the actual "variable" we will write data to
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // 5. Add a Descriptor (Required for notifications to work on Phone/Linux)
  pCharacteristic->addDescriptor(new BLE2902());

  // 6. Start the Service
  pService->start();

  // 7. Start Advertising (Broadcasting existence)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection...");
}

void loop() {
  // Only send data if something is actually connected
  if (deviceConnected) {
    
    // Create the message
    //
    int currentState = digitalRead(INPUT_PIN_D1);

    const char* status = (currentState == 1) ? "HIGH" : "LOW";
    char message[11];

    snprintf(message, sizeof(message), "BLE = %s", status);
    
    // Update the value
    pCharacteristic->setValue(message);
    
    // Send the notification (Push the data to the phone/PC)
    pCharacteristic->notify();
    
    Serial.println("Notification sent");
    delay(2000); // Send every 2 seconds
  }
}
