#pragma once

#include <string>
#include <vector>

namespace sniffles::capture {
struct DeviceInfo {
  std::string name;
  std::string description;
};

class DeviceEnumerator {
public:
  static bool ListDevices(std::vector<DeviceInfo> &devices,
                          std::string &error_message);
};
} // namespace sniffles::capture
