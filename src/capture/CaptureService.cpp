#include "capture/CaptureService.h"
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <utility>

namespace sniffles::capture {
bool CaptureService::Start(const std::string &device_name) {
  device_name_ = device_name;
  running_ = !device_name_.empty();
  pcpp::PcapLiveDeviceList *device_list;
  for (auto &live_device : *device_list) {
    if (device_name == live_device->getName()) {
      // we can start capture since the device name is valid
    }
  }
  running_ = false;
  // Capture the device

  return running_;
}

void CaptureService::Stop() { running_ = false; }

bool CaptureService::IsRunning() const { return running_; }
} // namespace sniffles::capture
