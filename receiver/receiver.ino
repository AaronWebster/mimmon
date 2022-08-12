#include <RH_RF95.h>
#include <SPI.h>

#include "board.h"
#include "packets.h"

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial) continue;

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) continue;
  while (!rf95.setFrequency(RF95_FREQ)) continue;
  rf95.setTxPower(23, false);
}

void loop() {
  if (!rf95.available()) return;

  Packet packet;
  uint8_t len = sizeof(packet);
  if (!rf95.recv(reinterpret_cast<uint8_t*>(&packet), &len)) return;

  Serial.print("station_id: ");
  Serial.print(packet.station_id);
  Serial.print(" sequence_id: ");
  Serial.print(packet.sequence_id);

  switch (packet.measurement_type) {
    case MeasurementType::kTemperature:
      Serial.print(" temperature: ");
      Serial.print(packet.degrees_celsius);
      break;
    case MeasurementType::kPowerStatus:
      Serial.print(" power_status: ");
      Serial.print(packet.power_good);
      break;
    default:
      Serial.print(" [unknown] ");
      Serial.print(packet.power_good);
      break;
  }

  Serial.print("rssi: ");
  Serial.println(rf95.lastRssi(), DEC);
}
