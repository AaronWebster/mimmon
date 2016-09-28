#include <Adafruit_SleepyDog.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

// AdaFruit Feather 32u4.
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define BATT 9

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

#define SONAR_TRIGGER 11

#define RED_LED 13

// Radio driver.  Defaults after init are 434.0MHz, 13dBm, Bw=125 kHz, Cr=4/5,
// Sf=128chips/symbol, CRC on.
RH_RF95 driver(RFM95_CS, RFM95_INT);

// Message manager.
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

// Measurement buffer.
uint8_t buf[6];

void setup() {
  pinMode(SONAR_TRIGGER, OUTPUT);
  digitalWrite(SONAR_TRIGGER, LOW);

  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);

  Serial1.begin(9600);

  // Indicate manager failure by blinking LEDs.
  if (!manager.init()) {
    while (true) {
      for (int i = 0; i < 3; ++i) {
        digitalWrite(RED_LED, HIGH);
        Watchdog.sleep(250);
        digitalWrite(RED_LED, LOW);
        Watchdog.sleep(250);
      }
      Watchdog.sleep(5000);
    }
  }

  // RFM95 transmitter power 23dBm, using PA_BOOST.
  driver.setTxPower(23, false);

  // Wait until Channel Activity Detection shows no activity before
  // transmitting.
  driver.setCADTimeout(10000);
}

void loop() {
  // Trigger sonar, read response.
  digitalWrite(SONAR_TRIGGER, HIGH);
  delayMicroseconds(50);
  digitalWrite(SONAR_TRIGGER, LOW);
  Serial1.readBytes(&buf[0], 6);

  // Send readings.
  manager.sendtoWait(buf, sizeof(buf), SERVER_ADDRESS);
  sprintf(buf, "B%04d\r", analogRead(BATT));
  manager.sendtoWait(buf, sizeof(buf), SERVER_ADDRESS);

  // Sleep until next reading.
  driver.sleep();
  int seconds_asleep = 0;
  while (seconds_asleep < 300) {
    seconds_asleep += Watchdog.sleep(5000) / 1000;
  }
}
