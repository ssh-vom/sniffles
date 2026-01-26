#pragma once

#include <cstddef>
#include <cstdint>

namespace sniffles::decode {
struct PacketView {
  const std::uint8_t *data = nullptr;
  std::size_t size = 0;
};
} // namespace sniffles::decode
