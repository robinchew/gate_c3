#include <WiFi.h>
#include <HTTPClient.h>

#define INPUT_PIN_D0 2
#define INPUT_PIN_D1 3

const int GATE_CLOSE_PIN = INPUT_PIN_D0;
const char* ssid = "TPG-EYVZ-B";
const char* password = "???";

void send(HTTPClient &http, String path) {
    http.begin("http://192.168.1.13:8080/" + path);
    http.GET();
    http.end();
}

void setup() {
    Serial.begin(115200); // This is required for print to work
    delay(1000);

    pinMode(GATE_CLOSE_PIN, INPUT_PULLUP);  // Internal pull-up
    esp_deep_sleep_enable_gpio_wakeup(1 << GATE_CLOSE_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    HTTPClient http;
    send(http, "close");
    send(http, "not-close");

    Serial.println("sleep");
    Serial.flush();
    esp_deep_sleep_start();
}

void loop() {}
