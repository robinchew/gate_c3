#include <WiFi.h>
#include <HTTPClient.h>

#define INPUT_PIN_D0 2
#define INPUT_PIN_D1 3

#define PIN_2_MASK (1ULL << 2)
#define PIN_3_MASK (1ULL << 3)

const int GATE_CLOSE_PIN = INPUT_PIN_D0;
const int GATE_NOT_CLOSE_PIN = INPUT_PIN_D1;

const char* ssid = "TPG-EYVZ-B";
const char* password = "???";

void send(HTTPClient &http, String path) {
    http.begin("http://192.168.1.13:8080/" + path);
    http.GET();
    http.end();
}

void read_all() {
    Serial.print("READ: ");
    Serial.print(analogRead(GATE_CLOSE_PIN) * (3.3 / 4095.0));
    Serial.print(" ");
    Serial.print(analogRead(GATE_NOT_CLOSE_PIN) * (3.3 / 4095.0));
    Serial.print(" ");
    Serial.print(digitalRead(GATE_CLOSE_PIN));
    Serial.print(" ");
    Serial.println(digitalRead(GATE_NOT_CLOSE_PIN));
}

void setup() {
    Serial.begin(115200); // This is required for print to work
    delay(1000);

    read_all();
    Serial.flush();

  uint32_t wakeup_reason = esp_sleep_get_wakeup_cause();

    Serial.print("Wake up initiated by GPIO mask: ");
    Serial.println(wakeup_reason);

    pinMode(GATE_CLOSE_PIN, INPUT_PULLDOWN);  // Internal pull-up
    pinMode(GATE_NOT_CLOSE_PIN, INPUT_PULLDOWN);  // Internal pull-up

    esp_deep_sleep_enable_gpio_wakeup(1 << GATE_CLOSE_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);
    esp_deep_sleep_enable_gpio_wakeup(1 << GATE_NOT_CLOSE_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);

    uint64_t wakeup_pin_mask = esp_sleep_get_gpio_wakeup_status();

    int close_pin_int = digitalRead(GATE_CLOSE_PIN);
    int not_close_pin_int = digitalRead(GATE_NOT_CLOSE_PIN);
    bool is_closed = close_pin_int == 0;
    bool is_not_closed = not_close_pin_int == 1;

    String pin_label = "";

    /*
    if (wakeup_pin_mask & PIN_0_MASK) {
      Serial.println("Woke up by GPIO 0");
    }
    else if (wakeup_pin_mask & PIN_1_MASK) {
      Serial.println("Woke up by GPIO 1");
    }
    */
    if (wakeup_pin_mask & PIN_2_MASK) {
      pin_label += "p2";
    }
    if (wakeup_pin_mask & PIN_3_MASK) {
      pin_label += "p3";
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    HTTPClient http;
    String debug_string = "debug/";
    send(http, debug_string + String(wakeup_pin_mask) + '/' +  pin_label + "/m/" + String(PIN_2_MASK) + '/' + String(PIN_3_MASK));
    /*
    if (is_closed) {
        send(http, "close");
    } else if (is_not_closed) {
        send(http, "not-close");
    }

    */
    // delay(3000);
   // read_all();
    Serial.println("sleep");
    Serial.flush();
    esp_deep_sleep_start();
}

void loop() {}
