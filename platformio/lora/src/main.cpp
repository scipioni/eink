#include <Arduino.h>
// Pause between sends in seconds, so this is every 15 minutes. (Delay will be
// longer if regulatory or TTN Fair Use Policy requires it.)
#define MINIMUM_DELAY 900

#define HELTEC_WIRELESS_STICK_LITE
#include <LoRaWAN_ESP32.h>
#include <RadioLib.h>
#include <heltec_unofficial.h>

LoRaWANNode *node;

RTC_DATA_ATTR uint8_t count = 0;

// Forward declaration of goToSleep function
void goToSleep();

void setup() {
  heltec_setup();

  // Obtain directly after deep sleep
  // May or may not reflect room temperature, sort of.
  float temp = heltec_temperature();
  Serial.printf("Temperature: %.1f °C\n", temp);

  // initialize radio
  Serial.println("Radio init");
  int16_t state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("Radio did not initialize. We'll try again later.");
    goToSleep();
  }

  node = persist.manage(&radio);

  if (!node->isActivated()) {
    Serial.println("Could not join network. We'll try again later.");
    goToSleep();
  }

  // If we're still here, it means we joined, and we can send something

  // Manages uplink intervals to the TTN Fair Use Policy
  node->setDutyCycle(true, 1250);

  uint8_t uplinkData[2];
  uplinkData[0] = count++;
  uplinkData[1] = temp + 100;

  uint8_t downlinkData[256];
  size_t lenDown = sizeof(downlinkData);

  state = node->sendReceive(uplinkData, sizeof(uplinkData), 1, downlinkData,
                            &lenDown);

  if (state == RADIOLIB_ERR_NONE || state == RADIOLIB_LORAWAN_NO_DOWNLINK) {
    Serial.println("Message sent");
  } else {
    Serial.printf("sendReceive returned error %d, we'll try again later.\n",
                  state);
  }

  goToSleep(); // Does not return, program starts over next round
}

void loop() {
  // This is never called. There is no repetition: we always go back to
  // deep sleep one way or the other at the end of setup()
}

void goToSleep() {
  Serial.println("Going to deep sleep now");
  // allows recall of the session after deepsleep
  persist.saveSession(node);
  // Calculate minimum duty cycle delay (per FUP & law!)
  uint32_t interval = node->timeUntilUplink();
  // And then pick it or our MINIMUM_DELAY, whichever is greater
  uint32_t delayMs = max(interval, (uint32_t)MINIMUM_DELAY * 1000);
  Serial.printf("Next TX in %i s\n", delayMs / 1000);
  delay(100); // So message prints
  // and off to bed we go
  heltec_deep_sleep(delayMs / 1000);
}
