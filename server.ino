#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// AdaFruit Feather 32u4.
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define RED_LED 13

// Radio driver.  Defaults after init are 434.0MHz, 13dBm, Bw=125 kHz, Cr=4/5,
// Sf=128chips/symbol, CRC on.
RH_RF95 driver(RFM95_CS, RFM95_INT);

// Message manager.
RHReliableDatagram manager(driver, SERVER_ADDRESS);

// Message buffer.
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void setup() {
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  Serial.begin(9600);
  // Wait for serial port to be available
  while (!Serial) delay(1);

  if (!manager.init()) Serial.println("Init failed!");
}

void loop() {
  if (manager.available()) {
    // Wait for a message addressed to from client.
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)) {
      digitalWrite(RED_LED, HIGH);
      Serial.print(from, DEC);
      Serial.print("\t");
      Serial.println((char*)buf);
      digitalWrite(RED_LED, LOW);
    }
  }
}
