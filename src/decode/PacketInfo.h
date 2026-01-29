#pragma once

#include "decode/FlowKey.h"
#include <chrono>
#include <string>
namespace sniffles::decode {
struct PacketInfo {
  std::chrono::system_clock::time_point timestamp; // Timing

  FlowKey flow; // Network info

  std::string protocol_name;
  uint8_t protocol_num;

  // Protocol info
  size_t length = 0;
  size_t captured_length = 0;

  // Packet metadata
  std::string summary;

  // raw data reference
  std::vector<uint8_t> raw_data;
};

} // namespace sniffles::decode
