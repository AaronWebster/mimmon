#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <cstdint>
#include <string>
#include <vector>

// #include "absl/cleanup/cleanup.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "logging.h"

ABSL_FLAG(std::string, serial_port, "/dev/ttyACM0",
          "Serial port to listen on.");
ABSL_FLAG(int, baud, 115200, "Serial baud, 8 bits, no parity.");

namespace mimmon {
namespace {

void SetSerialAttributes(int fd, int speed) {
  termios tty{};
  PCHECK(tcgetattr(fd, &tty) == 0);

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  // 8-bit chars and disable IGNBRK for mismatched speed tests; otherwise
  // receive break as \000 chars.
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_iflag &= ~IGNBRK; // Disable break processing.
  tty.c_lflag = 0; // Disable signaling chars, echo, and canonical processing.
  tty.c_oflag = 0; // Disable remapping and delays.
  tty.c_cc[VMIN] = 0;  // Read doesn't block.
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout.

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable xon/xoff ctrl.
  tty.c_cflag |= (CLOCAL | CREAD);   // Ignore modem controls, enable reading.
  tty.c_cflag &= ~(PARENB | PARODD); // Disable parity.
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  PCHECK(tcsetattr(fd, TCSANOW, &tty) == 0);
}

void SetSerialBlocking(int fd, bool blocking) {
  termios tty{};
  PCHECK(tcgetattr(fd, &tty) == 0);
  tty.c_cc[VMIN] = blocking ? 1 : 0;
  tty.c_cc[VTIME] = 5; // 0.5s.
  PCHECK(tcsetattr(fd, TCSANOW, &tty) == 0);
}

void Main() {
  const std::string serial_port = absl::GetFlag(FLAGS_serial_port);
  CHECK(!serial_port.empty());
  const int baud = absl::GetFlag(FLAGS_baud);
  CHECK(baud > 0);

  std::cerr << "Listening on " << serial_port << " " << baud << ",8n1"
            << std::endl;

  int fd = open(serial_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
  // auto fd_cleanup = absl::MakeCleanup([fd] { close(fd); });
  PCHECK(fd != -1);
  SetSerialBlocking(fd, true);
  SetSerialAttributes(fd, baud);

  std::vector<uint8_t> buf(1024);
  const int bytes_read = read(fd, buf.data(), buf.size());
  PCHECK(bytes_read != -1);
}

} // namespace
} // namespace mimmon

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  mimmon::Main();
  return EXIT_SUCCESS;
}
