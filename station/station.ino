#include "crc32c.h"
#include "heltec.h"
#include "messages.emb.h"

std::array<uint8_t, mimmon::Message::MaxSizeInBytes()> message_buf;
mimmon::MessageWriter message{&message_buf};
uint16_t sequence_id = 0;

void setup() {
  message_buf.fill(0);
  message.magic().Write(message.magic_value().Read());
  message.station_id().Write(1);

  Heltec.begin(/*DisplayEnable=*/true, /*LoRaEnable=*/true,
               /*SerialEnable=*/true, /*PABOOST=*/true, /*BAND=*/433e6);
  Heltec.display->init();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  delay(1500);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Haldo!.");
  Heltec.display->display();
  delay(1000);
}

void loop() {
  message.sequence_id().Write(std::max(message.sequence_id().Read() + 1, 1));

  crc_t crc;
  crc = crc_init();
  crc = crc_update(crc, message.crc_data().BackingStorage().data(),
                   message.crc_data().BackingStorage().SizeInBytes());
  crc = crc_finalize(crc);
  message.crc().Write(crc);

  // std::string result = emboss::WriteToString(message);
  Serial.write(message.BackingStorage().data(), message.SizeInBytes());

  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Sequence ID: ");
  Heltec.display->drawString(90, 0, String(message.sequence_id().Read()));
  Heltec.display->display();

  delay(1000);
}
