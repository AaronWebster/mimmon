#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

String kBaseUrl = "https://docs.google.com/forms/d/e/YOUR_SPREADSHEET_ID/formResponse?entry.ENTRY_CODE=";
// Client fingerprint, to generate, use:
//   openssl s_client -connect docs.google.com:443 < /dev/null 2>/dev/null | openssl x509 -fingerprint -noout -in /dev/stdin
constexpr char kFingerprint[] = "5B:E1:DA:24:FE:99:92:FB:D2:AB:A8:DD:E9:49:AF:3A:56:43:F1:B4";
constexpr char kApSsid[] = "YOUR_SSID";
constexpr char kApPassword[] = "";
constexpr int kPin = 5;

ESP8266WiFiMulti WiFiMulti;

int pulse_count = 0;
int previous_state = LOW;
int current_state;
unsigned long previous_debounce_ms = 0;
constexpr unsigned long kDebounceDelayMs = 50;

void setup() {
  pinMode(kPin, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

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
        client->setFingerprint(kFingerprint);

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
          Serial.println("Unable to connect.");
        }
      }
    }
  }
  previous_state = state;
}
