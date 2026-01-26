#pragma once

#include <string>

namespace sniffles::capture {
class BpfFilter {
public:
  explicit BpfFilter(std::string expression);
  const std::string &Expression() const;

private:
  std::string expression_;
};
} // namespace sniffles::capture
