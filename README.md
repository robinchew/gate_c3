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
