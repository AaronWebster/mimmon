#ifndef STATION_H_
#define STATION_H_

#include <memory>

#include "messages.emb.h"
#include "runtime/cpp/emboss_cpp_util.h"

namespace mimmon {

class Station {
 public:
  virtual ~Station() = default;

  static std::unique_ptr<Station> New(int station_id);

  virtual void HandleMessage(MessageView message) = 0;
};

}  // namespace mimmon

#endif  // STATION_H_
