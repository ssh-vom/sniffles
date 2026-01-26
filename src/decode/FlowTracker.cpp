#include "decode/FlowTracker.h"

namespace sniffles::decode {
void FlowTracker::Reset() {
  count_ = 0;
}

void FlowTracker::Increment() {
  ++count_;
}

std::size_t FlowTracker::FlowCount() const {
  return count_;
}
} // namespace sniffles::decode
