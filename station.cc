#include "station.h"

namespace mimmon {
namespace {

class StationImpl : public Station {
public:
  StationImpl(int station_id) : station_id_(station_id) {
    message_buf_.resize(Message::MaxSizeInBytes());
    message_ = MakeMessageView(&message_buf_);
  }

  void HandleMessage(MessageView message) override {
    if (message.source_station_id().Read() == station_id_)
      return;

    // Forward message, decrement TTL.
    if (message.destination_station_id().Read() != station_id_ &&
        message.ttl().Read() > 0) {
      message_.CopyFrom(message);
      message_.ttl().Write(message.ttl().Read() - 1);
      return;
    }
  }

private:
  const int station_id_;
  std::vector<uint8_t> message_buf_;
  MessageWriter message_;
};

} // namespace

std::unique_ptr<Station> Station::New(int station_id) {
  return std::make_unique<StationImpl>(station_id);
}

} // namespace mimmon
