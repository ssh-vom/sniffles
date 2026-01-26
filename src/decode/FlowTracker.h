#pragma once

#include <cstddef>

namespace sniffles::decode {
class FlowTracker {
public:
  void Reset();
  void Increment();
  std::size_t FlowCount() const;

private:
  std::size_t count_ = 0;
};
} // namespace sniffles::decode
