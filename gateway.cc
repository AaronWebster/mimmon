#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "absl/cleanup/cleanup.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/thread.h"
#include "logging.h"
#include "messages.emb.h"
#include "runtime/cpp/emboss_cpp_util.h"
#include "runtime/cpp/emboss_text_util.h"

ABSL_FLAG(std::string, serial_port, "/dev/ttyACM0",
          "Serial port to listen on.");

namespace mimmon {
namespace {

class Gateway {
public:
  Gateway(const std::string &serial_port, event_base *ev_base)
      : ev_base_(ev_base) {
    fd_ = open(serial_port.c_str(), O_RDWR | O_NOCTTY | O_CLOEXEC | O_NONBLOCK);
    PCHECK(fd_ != -1);

    termios tty{};
    cfmakeraw(&tty);
    tty.c_cflag = CS8 | CREAD | CLOCAL;
    cfsetspeed(&tty, B115200);
    PCHECK(tcsetattr(fd_, TCSANOW, &tty) != -1);
    PCHECK(tcflush(fd_, TCIOFLUSH) != -1);
    connected_ = true;

    connection_ = bufferevent_socket_new(ev_base_, fd_, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(connection_, ReadCallback, WriteCallback, EventCallback,
                      this);
    bufferevent_enable(connection_, EV_READ | EV_WRITE);
  }

  ~Gateway() {
    bufferevent_free(connection_);
    close(fd_);
  }

private:
  static void ReadCallback(bufferevent *bev, void *void_self) {
    auto self = static_cast<Gateway *>(void_self);

    std::vector<uint8_t> buf(1024);
    buf.resize(bufferevent_read(self->connection_, buf.data(), buf.size()));
    std::cout << buf.size() << std::endl;
  }

  static void WriteCallback(bufferevent *bev, void *void_self) {
    // auto self = static_cast<Gateway *>(void_self);
  }

  static void EventCallback(bufferevent *bev, int16_t events, void *void_self) {
    auto self = static_cast<Gateway *>(void_self);
    if (events & BEV_EVENT_CONNECTED) {
      std::cerr << "Connected." << std::endl;
      bufferevent_enable(self->connection_, EV_READ | EV_WRITE);
      return;
    }

    if (events & BEV_EVENT_EOF) {
      std::cerr << "Closed." << std::endl;
      return;
    }

    if (events & BEV_EVENT_ERROR) {
      std::cerr << "Error: "
                << evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR())
                << std::endl;
      return;
    }
  }

  event_base *ev_base_ = nullptr;
  int fd_ = -1;
  bufferevent *connection_ = nullptr;
  bool connected_ = false;
};

void Main() {
  const std::string serial_port = absl::GetFlag(FLAGS_serial_port);
  CHECK(!serial_port.empty());

  evthread_use_pthreads();
  event_base *ev_base = event_base_new();
  CHECK(ev_base != nullptr);
  auto event_base_cleanup =
      absl::MakeCleanup([ev_base] { event_base_free(ev_base); });

  event *sigint_event = evsignal_new(
      ev_base, SIGINT,
      [](int, int16_t, void *ev_base) {
        event_base_loopexit(static_cast<event_base *>(ev_base), nullptr);
      },
      ev_base);
  auto sigint_event_cleanup =
      absl::MakeCleanup([sigint_event] { event_free(sigint_event); });

  event *sigterm_event = evsignal_new(
      ev_base, SIGTERM,
      [](int, int16_t, void *ev_base) {
        event_base_loopexit(static_cast<event_base *>(ev_base), nullptr);
      },
      ev_base);
  auto sigterm_event_cleanup =
      absl::MakeCleanup([sigterm_event] { event_free(sigterm_event); });

  Gateway gateway(serial_port, ev_base);

  CHECK(!event_base_dispatch(ev_base));
  libevent_global_shutdown();
}

} // namespace
} // namespace mimmon

int main(int argc, char **argv) {
  absl::ParseCommandLine(argc, argv);
  mimmon::Main();
  return EXIT_SUCCESS;
}
