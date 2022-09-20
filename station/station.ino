#include "crc32c.h"
#include "heltec.h"
#include "messages.emb.h"

std::array<uint8_t, mimmon::Message::MaxSizeInBytes()> message_buf;
mimmon::MessageWriter message{&message_buf};
uint16_t sequence_id = 0;

void setup() {
  message_buf.fill(0);
  message.station_id().Write(1);
  message.target_type().Write(mimmon::TargetType::TANK);
  message.measurement_type().Write(mimmon::MeasurementType::PRESSURE);

  Heltec.begin(/*DisplayEnable=*/true, /*LoRaEnable=*/true,
               /*SerialEnable=*/true, /*PABOOST=*/true, /*BAND=*/433e6);
  Heltec.display->init();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "mimmon v1.0");
  Heltec.display->display();
  delay(1000);
}

void WriteCrc(mimmon::MessageWriter message) {
  message.crc().Write(0);
  crc_t crc = crc_init();
  crc = crc_update(crc, message.BackingStorage().data(),
                   message.SizeInBytes() - message.crc().SizeInBits() / 8);
  crc = crc_finalize(crc);
  message.crc().Write(crc);
}

void loop() {
  message.sequence_id().Write(std::max(message.sequence_id().Read() + 1, 1));
  message.measurement_value().Write(random(0, 10000) / 100.0);
  WriteCrc(message);

  Serial.write(message.BackingStorage().data(), message.SizeInBytes());

  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "mimmon v1.0");
  Heltec.display->drawString(0, 10,
                             "seq:   " + String(message.sequence_id().Read()));
  Heltec.display->drawString(
      0, 20, "value: " + String(message.measurement_value().Read()));
  Heltec.display->display();

  delay(1000);
}
