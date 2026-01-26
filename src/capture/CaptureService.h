#pragma once

#include <string>

namespace sniffles::capture {
class CaptureService {
public:
  bool Start(const std::string &device_name);
  void Stop();
  bool IsRunning() const;

private:
  bool running_ = false;
  std::string device_name_;
};
} // namespace sniffles::capture
