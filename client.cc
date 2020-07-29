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
#include <string>
#include <vector>

#define CHECK(ret)                                                           \
  do {                                                                       \
    if (ret == -1) {                                                         \
      std::cout << __FILE__ << ":" << __LINE__ << "] " << std::strerror(ret) \
                << " [" << ret << "]";                                       \
      close(fd);                                                             \
      return ret;                                                            \
    }                                                                        \
  } while (0)

int main(int argc, char** argv) {
  const std::string interface = "eth0";
  const std::string ip = "192.168.4.255";
  const uint16_t port = 11222;

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  CHECK(fd);

  constexpr int kTrue = 1;
  CHECK(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &kTrue, sizeof(kTrue)));
  CHECK(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &kTrue, sizeof(kTrue)));
  CHECK(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, interface.c_str(),
                   interface.size()));

  sockaddr socket_address{};
  socklen_t socket_address_size = 0;
  sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&socket_address);
  CHECK(inet_pton(AF_INET, ip.c_str(), &addr->sin_addr));
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  socket_address_size = sizeof(*addr);
  CHECK(bind(fd, &socket_address, socket_address_size));

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

  while (true) {
    int bytes_read = recvmsg(fd, &header, 0);
    if (bytes_read > 0) {
      std::cout << buf << std::endl;
    }
  }

  close(fd);
}
