
enum class MeasurementType : uint8_t { kInvalid, kTemperature, kPowerStatus };

struct Packet {
  uint8_t station_id;
  uint32_t sequence_id;
  MeasurementType measurement_type;
  union {
    float degrees_celsius;
    bool power_good;
  };
} __attribute__((packed));
