#include "util/Logging.h"

#include <iostream>

namespace sniffles::util {
namespace {
const char *ToLabel(LogLevel level) {
  switch (level) {
  case LogLevel::kInfo:
    return "INFO";
  case LogLevel::kWarning:
    return "WARN";
  case LogLevel::kError:
    return "ERROR";
  }
  return "LOG";
}
} // namespace

void Log(LogLevel level, const std::string &message) {
  std::cerr << "[" << ToLabel(level) << "] " << message << "\n";
}
} // namespace sniffles::util
