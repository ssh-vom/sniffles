#include "decode/Decoders.h"

#include <string>

namespace sniffles::decode {
DecodeResult DecodePacketSummary(const std::string &bytes) {
  if (bytes.empty()) {
    return {false, "empty packet"};
  }

  return {true, "payload bytes: " + std::to_string(bytes.size())};
}
} // namespace sniffles::decode
