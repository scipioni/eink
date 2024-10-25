// #include <Arduino.h>
//  Pause between sends in seconds, so this is every 15 minutes. (Delay will be
//  longer if regulatory or TTN Fair Use Policy requires it.)

#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>

#define HELTEC_WIRELESS_STICK_LITE
// Turns the 'PRG' button into the power button, long press is off
#define HELTEC_POWER_BUTTON // must be before "#include <heltec_unofficial.h>"
#define HELTEC_NO_DISPLAY_INSTANCE
#include <heltec_unofficial.h>

// Frequency in MHz. Keep the decimal point to designate float.
// Check your own rules and regulations to see what is legal where you are.
#define FREQUENCY 866.3 // for Europe
// #define FREQUENCY           905.2       // for US

// LoRa bandwidth. Keep the decimal point to designate float.
// Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and
// 500.0 kHz.
#define BANDWIDTH 250.0
// Number from 5 to 12. Higher means slower but higher "processor gain",
// meaning (in nutshell) longer range and more robust against interference.
#define SPREADING_FACTOR 9

// Transmit power in dBm. 0 dBm = 1 mW, enough for tabletop-testing. This
// value can be set anywhere between -9 dBm (0.125 mW) to 22 dBm (158 mW).
// Note that the maximum ERP (which is what your antenna maximally radiates)
// on the EU ISM band is 25 mW, and that transmissting without an antenna
// can damage your hardware. #define TRANSMIT_POWER 2

String rxdata;
String txdata;
volatile bool rxFlag = false;
long counter = 0;
uint64_t last_tx = 0;
uint64_t tx_time;
uint64_t minimum_pause;

WebServer server(80);
StaticJsonDocument<250> jsonDocument;
char buffer[250];

void wifi_init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void create_json(char *tag, float value, char *unit) {
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit;
}
void get_status() {
  jsonDocument.clear();
  JsonObject obj = jsonDocument.createNestedObject();
  obj["status"] = "ok";
  // add_json_object("temperature", temperature, "°C");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void handlePost() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  // action
  // int red_value = jsonDocument["red"];

  server.send(200, "application/json", "{}");
}
void setup_routing() {
  server.on("/", get_status);
  server.on("/msg", HTTP_POST, handlePost);

  server.begin();
}

void rx() { rxFlag = true; }

void setup() {
  heltec_setup();
  both.println("Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  // Set the callback function for received packets
  radio.setDio1Action(rx);
  // Set radio parameters
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  heltec_led(50); // 50% brightness is plenty for this LED
  wifi_init();
}

void loop() {
  heltec_loop();

  if (button.isSingleClick()) {
    heltec_led(0);
    counter++;
    // both.printf("TX [%s] ", String(counter).c_str());
    txdata = String(counter).c_str();
    radio.clearDio1Action();
    tx_time = millis();
    // RADIOLIB(radio.transmit(String(counter++).c_str()));
    // RADIOLIB(radio.transmit(txdata));
    _radiolib_status = radio.transmit(txdata);
    tx_time = millis() - tx_time;
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("TX='%s' (%i ms)\n", txdata, (int)tx_time);
    } else {
      both.printf("fail (%i)\n", _radiolib_status);
    }
    // Maximum 1% duty cycle
    minimum_pause = tx_time * 100;
    last_tx = millis();
    radio.setDio1Action(rx);
    radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
    // RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }

  // If a packet was received, display it and the RSSI and SNR
  if (rxFlag) {
    heltec_led(100); // 50% brightness is plenty for this LED
    rxFlag = false;
    radio.readData(rxdata);
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("RX='%s' RSSI:%.2fdBm SNR:%.2fdB\n", rxdata.c_str(),
                  radio.getRSSI(), radio.getSNR());
    }
    radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
    // RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}
