#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/net_tstamp.h>
#include <linux/sockios.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <poll.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"

ABSL_FLAG(std::string, interface, "", "Network interface.");
ABSL_FLAG(std::string, address, "", "Listen address interface.");

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  const std::string interface = absl::GetFlag(FLAGS_interface);
  ABSL_RAW_CHECK(!interface.empty(), "Missing required --interface.");

  const std::string address = absl::GetFlag(FLAGS_address);
  ABSL_RAW_CHECK(!address.empty(), "Missing required --address.");

  sockaddr socket_address{};
  socklen_t socket_address_size = 0;

  constexpr int kTrue = 1;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  ABSL_RAW_CHECK(
      fd != -1,
      absl::StrCat("Could not open socket: ", std::strerror(errno)).c_str());
  setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &kTrue, sizeof(kTrue));
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &kTrue, sizeof(kTrue));
  setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, interface.c_str(),
             interface.size());

  sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&socket_address);
  std::pair<std::string, std::string> ip_and_port_str =
      absl::StrSplit(address, ':');

  int port = -1;
  ABSL_RAW_CHECK(absl::SimpleAtoi(ip_and_port_str.second, &port), "");
  ABSL_RAW_CHECK(
      inet_pton(AF_INET, ip_and_port_str.first.c_str(), &addr->sin_addr) != -1,
      "");
  addr->sin_family = AF_INET;
  addr->sin_port = htons(static_cast<uint16_t>(port));
  socket_address_size = sizeof(*addr);

  ABSL_RAW_CHECK(bind(fd, &socket_address, socket_address_size) != -1, "");

  msghdr header{};
  sockaddr_storage source_address{};
  header.msg_name = &source_address;
  header.msg_namelen = sizeof(source_address);

  std::string buf(1024, 0);
  iovec iov{};
  header.msg_iov = &iov;
  header.msg_iovlen = 1;
  iov.iov_base = &buf[0];
  iov.iov_len = buf.size();
  ABSL_RAW_CHECK(recvmsg(fd, &header, 0) > 0, "");

  std::cout << buf << std::endl;

  close(fd);
}
