#pragma once

#include <cstdint>
#include <string>

namespace sniffles::decode {
struct FlowKey {
  std::string src;
  std::string dst;
  std::uint16_t src_port = 0;
  std::uint16_t dst_port = 0;
};
} // namespace sniffles::decode
