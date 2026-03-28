#pragma once

#include "decode/PacketInfo.h"
#include "util/ThreadSafeQueue.h"
#include <PcapDevice.h>
#include <PcapLiveDevice.h>
#include <boost/circular_buffer.hpp>
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
  uint64_t packets_dropped_by_buffer = 0;
};

class CaptureService {
public:
  explicit CaptureService(std::size_t buffer_capacity = 10000);

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
  pcpp::PcapLiveDevice *device_ = nullptr;
  util::ThreadSafeQueue<decode::PacketInfo> packet_queue_;
  boost::circular_buffer<decode::PacketInfo> packet_buffer_;
  CaptureStats stats_;

  static void OnPacketArrives(pcpp::RawPacket *packet,
                              pcpp::PcapLiveDevice *device, void *user_data);
  void HandlePacket(pcpp::RawPacket *packet);
  std::string GetProtocolName(uint8_t protocol);
  void FlushBufferToQueue();
};
} // namespace sniffles::capture
// namespace sniffles::capture
