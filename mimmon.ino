#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP_EEPROM.h>
#include <WiFiClientSecureBearSSL.h>

String kBaseUrl = "https://docs.google.com/forms/d/e/YOUR_SPREADSHEET_ID/formResponse?entry.ENTRY_CODE=";

constexpr char kApSsid[] = "YOUR_SSID";
constexpr char kApPassword[] = "";

constexpr int kPin = 5;
constexpr uint32_t kDebounceDelayMs = 50;

constexpr uint64_t kPulseValid = 0x5c6b6f2dabcf6948;
constexpr uint64_t kInitialPulseCount = 5542;
constexpr int kPulseCountAddress = 0;
constexpr int kPulseValidAddress = 8;

ESP8266WiFiMulti wifi;

uint32_t pulse_count = 0;
int previous_state = LOW;
int current_state;
uint32_t previous_debounce_ms = 0;

String uint64ToString(uint64_t input, uint8_t base) {
  String result = "";
  if (base < 2) base = 10;
  if (base > 36) base = 10;
  result.reserve(16);

  do {
    char c = input % base;
    input /= base;
    c += (c < 10) ? '0' : ('A' - 10);
    result = c + result;
  } while (input);
  return result;
}

void setup() {
  pinMode(kPin, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  delay(4000);

  WiFi.mode(WIFI_STA);
  wifi.addAP(kApSsid, kApPassword);
  while (wifi.run() != WL_CONNECTED) {
    Serial.println("Connecting Wifi...");
    Serial.flush();
    delay(1000);
  }

  Serial.println("");
  Serial.printf("Connected to %s\n", kApSsid);
  Serial.printf("IP address: ");
  Serial.println(WiFi.localIP());

  EEPROM.begin(16);

  uint64_t pulse_valid;
  EEPROM.get(kPulseValidAddress, pulse_valid);
  if (pulse_valid == kPulseValid) {
    EEPROM.get(kPulseCountAddress, pulse_count);
    Serial.printf("Loaded valid pulse count value of %d\n", pulse_count);
  } else {
    pulse_count = kInitialPulseCount;
    pulse_valid = kPulseValid;
    EEPROM.put(kPulseCountAddress, pulse_count);
    EEPROM.put(kPulseValidAddress, pulse_valid);
    EEPROM.commit();
    Serial.printf("Pulse count initialized to %d.", pulse_count);
  }
}

void loop() {
  if ((wifi.run() != WL_CONNECTED)) return;

  int state = digitalRead(kPin);
  if (state != previous_state) previous_debounce_ms = millis();

  if ((millis() - previous_debounce_ms) > kDebounceDelayMs) {
    if (state != current_state) {
      current_state = state;
      if (current_state == LOW) {
        ++pulse_count;
        Serial.printf("Pulse count: %d\n", pulse_count);
        std::unique_ptr<BearSSL::WiFiClientSecure> client(
            new BearSSL::WiFiClientSecure);
        client->setInsecure();

        HTTPClient https;
        if (https.begin(*client, kBaseUrl + uint64ToString(pulse_count, 10))) {
          int httpCode = https.GET();
          if (httpCode > 0) {
            if (!(httpCode == HTTP_CODE_OK ||
                  httpCode == HTTP_CODE_MOVED_PERMANENTLY)) {
              Serial.printf("HTTPS GET failed, code %d\n", httpCode);
            }
          }
          https.end();
        } else {
          Serial.println("Unable to connect");
        }
        EEPROM.put(0, pulse_count);
        EEPROM.commit();
      }
    }
  }
  previous_state = state;
}
