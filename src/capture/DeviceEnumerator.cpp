#include "capture/DeviceEnumerator.h"

#include <PcapLiveDeviceList.h>
#include <utility>

namespace sniffles::capture {
bool DeviceEnumerator::ListDevices(std::vector<DeviceInfo> &devices,
                                   std::string &error_message) {
  devices.clear();
  error_message.clear();

  pcpp::PcapLiveDeviceList &device_list =
      pcpp::PcapLiveDeviceList::getInstance();
  std::vector<pcpp::PcapLiveDevice *> found =
      device_list.getPcapLiveDevicesList();

  if (found.empty()) {
    error_message =
        "No capture interfaces found. Check permissions (try sudo) or "
        "ensure libpcap/pcapplusplus is installed.";
    return false;
  }

  devices.reserve(found.size());
  for (pcpp::PcapLiveDevice *device : found) {
    DeviceInfo info;
    info.name = device->getName();
    info.description = device->getDesc();
    if (info.description.empty()) {
      info.description = "No description";
    }
    devices.push_back(std::move(info));
  }

  return true;
}
} // namespace sniffles::capture
