#pragma once

#include <string>

namespace sniffles::util {
enum class LogLevel {
  kInfo,
  kWarning,
  kError
};

void Log(LogLevel level, const std::string &message);
} // namespace sniffles::util
