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
#include "absl/cleanup/cleanup.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "logging.h"
#include "messages.emb.h"
#include "runtime/cpp/emboss_cpp_util.h"
#include "runtime/cpp/emboss_text_util.h"

ABSL_FLAG(std::string, serial_port, "/dev/ttyACM0",
          "Serial port to listen on.");
ABSL_FLAG(int, baud, 115200, "Serial baud, 8 bits, no parity.");

namespace mimmon {
namespace {

// Sets serial attributes.  Returns 0 on success, otherwise -1 and errno
// is set.
int SetSerialAttributes(int fd, int baud) {
  termios tty{};
  int status = tcgetattr(fd, &tty);
  if (status != 0) return status;

  cfsetospeed(&tty, baud);
  cfsetispeed(&tty, baud);

  // 8-bit chars and disable IGNBRK for mismatched speed tests; otherwise
  // receive break as \000 chars.
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_iflag &= ~IGNBRK;  // Disable break processing.
  tty.c_lflag = 0;  // Disable signaling chars, echo, and canonical processing.
  tty.c_oflag = 0;  // Disable remapping and delays.
  tty.c_cc[VMIN] = 0;   // Read doesn't block.
  tty.c_cc[VTIME] = 5;  // 0.5 seconds read timeout.

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // Disable xon/xoff ctrl.
  tty.c_cflag |= (CLOCAL | CREAD);    // Ignore modem controls, enable reading.
  tty.c_cflag &= ~(PARENB | PARODD);  // Disable parity.
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  tty.c_cc[VMIN] = 1;   // Blocking.
  tty.c_cc[VTIME] = 5;  // 0.5s.

  return tcsetattr(fd, TCSANOW, &tty);
}

void Main() {
  const std::string serial_port = absl::GetFlag(FLAGS_serial_port);
  CHECK(!serial_port.empty());
  const int baud = absl::GetFlag(FLAGS_baud);
  CHECK(baud > 0);

  std::cerr << "Listening on " << serial_port << " " << baud << ",8n1"
            << std::endl;

  int fd = open(serial_port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
  auto fd_cleanup = absl::MakeCleanup([fd] { close(fd); });
  PCHECK(fd != -1);
  PCHECK(SetSerialAttributes(fd, baud) == 0);

  std::vector<uint8_t> message_buf(1024);
  auto message = MakeMessageView(&message_buf);
  const int bytes_read = read(fd, message_buf.data(), message_buf.size());
  PCHECK(bytes_read != -1);
  std::cerr << emboss::WriteToString(message) << std::endl;
}

}  // namespace
}  // namespace mimmon

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  mimmon::Main();
  return EXIT_SUCCESS;
}
