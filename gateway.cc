#include <iostream>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "event2/event.h"
#include "glog/logging.h"

ABSL_FLAG(std::string, serial_port, "/dev/ttyACM0",
          "Serial port to listen on.");

namespace mimmon {
namespace {

void Main() {
  const std::string serial_port = absl::GetFlag(FLAGS_serial_port);
  LOG(INFO) << serial_port;
}

} // namespace
} // namespace mimmon

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  mimmon::Main();
  return EXIT_SUCCESS;
}
