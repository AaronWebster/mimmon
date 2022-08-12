#include <RH_RF95.h>
#include <SPI.h>

#include "board.h"
#include "packets.h"

RH_RF95 rf95(RFM95_CS, RFM95_INT);

Packet packet{};

void setup() {
  packet.station_id = 1;

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) continue;
  while (!rf95.setFrequency(RF95_FREQ)) continue;
  rf95.setTxPower(23, false);
}

void loop() {
  while (++packet.sequence_id == 0) continue;

  if (packet.sequence_id % 2 == 0) {
    packet.measurement_type = MeasurementType::kTemperature;
    packet.degrees_celsius = random(10, 30) + random(0, 9) * 1e-1f;
  } else {
    packet.measurement_type = MeasurementType::kPowerStatus;
    packet.power_good = true;
  }

  rf95.send(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
  rf95.waitPacketSent();
  delay(1000);
}
