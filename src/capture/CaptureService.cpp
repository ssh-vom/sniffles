#include "capture/CaptureService.h"

namespace sniffles::capture {
bool CaptureService::Start(const std::string &device_name) {
  device_name_ = device_name;
  running_ = !device_name_.empty();
  return running_;
}

void CaptureService::Stop() {
  running_ = false;
}

bool CaptureService::IsRunning() const {
  return running_;
}
} // namespace sniffles::capture
