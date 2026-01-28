#pragma once

#include <PcapLiveDevice.h>
#include <functional>
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
  pcpp::PcapLiveDevice *device_ = nullptr; // pointer starts off null
  std::function<void(pcpp::RawPacket *)> packet_callback;
  // static required for c-style function callback
  static void OnPacketArrives(pcpp::RawPacket *packet,
                              pcpp::PcapLiveDevice *device, void *user_data);
  void HandlePacket(pcpp::RawPacket *packet);
};
} // namespace sniffles::capture
// namespace sniffles::capture
