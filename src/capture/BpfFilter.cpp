#include "capture/BpfFilter.h"

#include <utility>

namespace sniffles::capture {
BpfFilter::BpfFilter(std::string expression)
    : expression_(std::move(expression)) {}

const std::string &BpfFilter::Expression() const {
  return expression_;
}
} // namespace sniffles::capture
