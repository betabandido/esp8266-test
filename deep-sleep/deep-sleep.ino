#include <EEPROM.h>
#include <PrintEx.h>

// See "ESP8266 Low Power Solutions" for details
// (https://espressif.com/en/file/855/download?token=u3bgWX4C)

extern "C" {
  #include <user_interface.h>
}

// Active time: 10s (measured in ms)
static constexpr unsigned active_time = 10 * 1000; 
// Sleep time: 10s (measured in us)
static constexpr unsigned sleep_time = 10 * 1000 * 1000;

static constexpr unsigned sleep_options[] = { 0, 1, 2, 4 };
static constexpr unsigned num_sleep_options =
    sizeof(sleep_options) / sizeof(sleep_options[0]);
static uint8_t curr_sleep_option_idx = 0;

PrintEx serial = Serial;

void setup() {
  Serial.begin(115200);
  delay(500);

  EEPROM.begin(1);
  curr_sleep_option_idx = EEPROM.read(0);
  if (curr_sleep_option_idx >= num_sleep_options) {
    // 1st time intialization
    curr_sleep_option_idx = 0;
  }

  uint8_t next_sleep_option_idx =
      (curr_sleep_option_idx + 1) % num_sleep_options;
  EEPROM.write(0, next_sleep_option_idx);
  EEPROM.commit();
}

void loop() {
  Serial.println("\nProgram started");
  delay(active_time);

  auto curr_sleep_option = sleep_options[curr_sleep_option_idx];

  serial.printf("Going to deep sleep, option=%d\n", curr_sleep_option);
  system_deep_sleep_set_option(curr_sleep_option);
  system_deep_sleep(sleep_time);

  // Apparently ESP8266 does not immediately go to deep sleep.
  Serial.println("Waiting for deep sleep");
  while (true) {
    Serial.print(".");
    delay(1000);
  }
}

