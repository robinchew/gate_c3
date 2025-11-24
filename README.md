Quick Start
===========

Create platformio project
-------------------------
```bash
pio boards <search_term>
pin boards esp32c3

pin project init --board seeed_xiao_esp32c3

cat > src/main.cpp <<EOF
#include <WiFi.h>

void setup() {
    Serial.begin(115200); // This is required for print to work
    Serial.print("c3 SETUP\n");
}

void loop() {
    Serial.print("c3 loop\n");
    delay(1000);
}
EOF

cat > flash_and_monitor.sh <<EOF
set -e # Ensures that if upload fails due to code errors, monitor does not run
pio run --target upload
pio device monitor
EOF
```

Reset on monitor
----------------
Running monitor like so, will reboot the device and allow you to see outputs from the `setup` function.

```bash
pio device monitor --dtr 0 --rts 0
```

Both `--dtr` and `--rts` needs to be set 0. Having one zero and the other non-zero does not have any usecase for us.

Troubleshooting
===============
Error
-----
```cpp
#include <WiFi.h>

#define WAKEUP_PIN 3  // Assuming D0 is GPIO 0

void setup() {
  Serial.begin(115200); // This is required for print to work
  pinMode(WAKEUP_PIN, INPUT_PULLUP);  // Internal pull-up
  esp_deep_sleep_enable_gpio_wakeup(1 << WAKEUP_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_deep_sleep_start();
}

void loop() {}
```

Code above looks like it should work but does not. Errors look like:

wait usb download
Disconnected (device reports readiness to read but returned no data (device disconnected or multiple access on port?))

Reset button stops working.
Input wakeup does not work.

Fix
---
Add delay after Serial.begin.

```cpp
void setup() {
  Serial.begin(115200); // This is required for print to work
  delay(1000);
  // ...
}
```

Missing prints
--------------
Fix is to do flush afterwards:
```cpp
Serial.println("sleep");
Serial.flush();
```
