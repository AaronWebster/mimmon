#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <cstdint>
#include <string>
#include <vector>

#include "absl/cleanup/cleanup.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "event2/event.h"
#include "event2/thread.h"
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
  if (status != 0)
    return status;

  cfsetospeed(&tty, baud);
  cfsetispeed(&tty, baud);

  // 8-bit chars and disable IGNBRK for mismatched speed tests; otherwise
  // receive break as \000 chars.
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  tty.c_iflag &= ~IGNBRK; // Disable break processing.
  tty.c_lflag = 0; // Disable signaling chars, echo, and canonical processing.
  tty.c_oflag = 0; // Disable remapping and delays.
  tty.c_cc[VMIN] = 33;  // Read blocks.
  tty.c_cc[VTIME] = 10; // 1s timeout.

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable xon/xoff ctrl.
  tty.c_cflag |= (CLOCAL | CREAD);   // Ignore modem controls, enable reading.
  tty.c_cflag &= ~(PARENB | PARODD); // Disable parity.
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  return tcsetattr(fd, TCSANOW, &tty);
}

void Main() {

  evthread_use_pthreads();
  event_base *ev_base = event_base_new();
  auto event_base_cleanup =
      absl::MakeCleanup([ev_base] { event_base_free(ev_base); });

  const std::string serial_port = absl::GetFlag(FLAGS_serial_port);
  CHECK(!serial_port.empty());
  const int baud = absl::GetFlag(FLAGS_baud);
  CHECK(baud > 0);

  std::cerr << "Listening on " << serial_port << " " << baud << ",8n1"
            << std::endl;

  int fd =
      open(serial_port.c_str(), O_RDWR | O_NOCTTY | O_CLOEXEC | O_NONBLOCK);
  auto fd_cleanup = absl::MakeCleanup([fd] { close(fd); });
  PCHECK(fd != -1);
  PCHECK(SetSerialAttributes(fd, baud) == 0);

  event_base_dispatch(ev_base);
  // std::string buf(1024,0);;
  // int pos = 0;
  // for (;;) {
  //   const int bytes_read = read(fd, buf.data() + pos, buf.size() - pos);
  //   PCHECK(bytes_read != -1);
  //   pos += bytes_read;
  //   if(pos < 2* Message::MaxSizeInBytes()) continue;

  // std::cout << buf <<std::endl;
  // buf.resize(bytes_read);
  // message_buf.insert(message_buf.end(), buf.begin(), buf.end());

  // auto pos = buf.find("mimmon");
  // std::cout << pos << std::endl;
  // if (pos == std::string::npos) continue;
  // if (message_buf.size() - pos < mimmon::Message::MaxSizeInBytes()) continue;
  // auto message = MakeMessageView(message_buf.data() + pos,
  //                                mimmon::Message::MaxSizeInBytes());
  // std::cerr << emboss::WriteToString(message) << std::endl;
  // }
}

} // namespace
} // namespace mimmon

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  mimmon::Main();
  return EXIT_SUCCESS;
}
