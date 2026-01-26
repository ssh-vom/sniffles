#include "util/Time.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace sniffles::util {
std::string NowIso8601() {
  auto now = std::chrono::system_clock::now();
  std::time_t time_value = std::chrono::system_clock::to_time_t(now);
  std::tm local_tm = *std::localtime(&time_value);

  std::ostringstream out;
  out << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S");
  return out.str();
}
} // namespace sniffles::util
