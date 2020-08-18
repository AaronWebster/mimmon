#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// Base URL for HTTP GET requests.
String kBaseUrl = "https://docs.google.com/forms/d/e/YOUR_SPREADSHEET_ID/formResponse?entry.ENTRY_CODE=";
// WiFi SSID and password.
constexpr char kApSsid[] = "YOUR_SSID";
constexpr char kApPassword[] = "";
// Pin to read meter pulse.
constexpr int kPin = 5;
// Debounce delay.
constexpr uint32_t kDebounceDelayMs = 50;

ESP8266WiFiMulti WiFiMulti;

int pulse_count = 0;
int previous_state = LOW;
int current_state;
uint32_t previous_debounce_ms = 0;

void setup() {
  pinMode(kPin, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  delay(4000);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(kApSsid, kApPassword);
}

void loop() {
  if ((WiFiMulti.run() != WL_CONNECTED)) return;
  int state = digitalRead(kPin);

  if (state != previous_state) {
    previous_debounce_ms = millis();
  }

  if ((millis() - previous_debounce_ms) > kDebounceDelayMs) {
    if (state != current_state) {
      current_state = state;
      if (current_state == LOW) {
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setInsecure();

        HTTPClient https;
        if (https.begin(*client, kBaseUrl + String(++pulse_count))) {
          int httpCode = https.GET();
          if (httpCode > 0) {
            if (!(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)) {
              Serial.println("HTTPS GET failed.");
            }
          }
          https.end();
        } else {
          Serial.println("Connection failed.");
        }
      }
    }
  }
  previous_state = state;
}
