#pragma once

#include <string>

namespace sniffles::decode {
struct DecodeResult {
  bool ok = false;
  std::string summary;
};

DecodeResult DecodePacketSummary(const std::string &bytes);
} // namespace sniffles::decode
