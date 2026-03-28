#pragma once

#include "decode/PacketInfo.h"
#include "util/ThreadSafeQueue.h"
#include <PcapDevice.h>
#include <PcapLiveDevice.h>
#include <string>

namespace sniffles::capture {

struct CaptureRequest {
  std::string device_name;
  std::string filter_expression;
};

struct CaptureStats {
  uint64_t packets_received = 0;
  uint64_t packets_dropped = 0;
  uint64_t packets_dropped_by_interface = 0;
};

class CaptureService {
public:
  bool Start(const CaptureRequest &request);
  void Stop();
  bool IsRunning() const;
  CaptureStats GetStats() const;
  util::ThreadSafeQueue<decode::PacketInfo> &GetPacketQueue() {
    return packet_queue_;
  }

private:
  bool running_ = false;
  std::string device_name_;
  std::string filter_expression_;
  pcpp::PcapLiveDevice *device_ = nullptr; // pointer starts off null
  util::ThreadSafeQueue<decode::PacketInfo> packet_queue_;
  static void OnPacketArrives(pcpp::RawPacket *packet,
                              pcpp::PcapLiveDevice *device, void *user_data);
  void HandlePacket(pcpp::RawPacket *packet);
  std::string GetProtocolName(uint8_t protocol);
};
} // namespace sniffles::capture
// namespace sniffles::capture
