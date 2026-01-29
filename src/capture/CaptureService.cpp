#include "capture/CaptureService.h"
#include <IPv4Layer.h>
#include <Packet.h>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <RawPacket.h>
#include <TcpLayer.h>
#include <UDPLayer.h>
#include <chrono>
#include <sys/time.h>

namespace sniffles::capture {
bool CaptureService::Start(const std::string &device_name) {
  device_name_ = device_name;

  pcpp::PcapLiveDeviceList &device_list =
      pcpp::PcapLiveDeviceList::getInstance();
  device_ = device_list.getDeviceByName(device_name_);

  if (device_ == nullptr)
    return false;

  if (!device_->open()) { // if we can't open the device
    device_ = nullptr;
    return false; // failed
  }

  if (!device_->startCapture(OnPacketArrives, this)) {
    device_->close();
    device_ = nullptr;
    return false;
  }

  running_ = true;

  return true;
}

std::string CaptureService::GetProtocolName(uint8_t protocol) {
  switch (protocol) {
  case 1:
    return "ICMP";
  case 6:
    return "TCP";
  case 17:
    return "UDP";
  case 2:
    return "IGMP";
  case 47:
    return "GRE";
  case 50:
    return "ESP";
  case 51:
    return "AH";
  default:
    return "Unknown (" + std::to_string(protocol) + ")";
  }
}

void CaptureService::Stop() {

  if (running_ && device_ != nullptr) {
    device_->stopCapture();
    device_->close();
  }

  running_ = false;
  device_ = nullptr;
}

bool CaptureService::IsRunning() const { return running_; }
void CaptureService::OnPacketArrives(pcpp::RawPacket *packet,
                                     pcpp::PcapLiveDevice *device,
                                     void *user_data) {

  CaptureService *self = static_cast<CaptureService *>(user_data);
  self->HandlePacket(packet);
}
void CaptureService::HandlePacket(pcpp::RawPacket *packet) {

  decode::PacketInfo info;
  timespec ts = packet->getPacketTimeStamp();
  info.timestamp = std::chrono::system_clock::from_time_t(ts.tv_sec);
  info.length = packet->getFrameLength();
  info.captured_length = packet->getRawDataLen();

  pcpp::Packet parsedPacket(packet);

  if (parsedPacket.isPacketOfType(pcpp::IPv4)) {
    pcpp::IPv4Layer *ip_layer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
    if (ip_layer) {
      info.flow.src = ip_layer->getSrcIPAddress().toString();
      info.flow.dst = ip_layer->getDstIPAddress().toString();
      info.protocol_num = ip_layer->getIPv4Header()->protocol;
      info.protocol_name = GetProtocolName(info.protocol_num);
    }
  }
  if (parsedPacket.isPacketOfType(pcpp::TCP)) {
    pcpp::TcpLayer *tcp_layer = parsedPacket.getLayerOfType<pcpp::TcpLayer>();
    if (tcp_layer) {
      info.flow.src_port = tcp_layer->getSrcPort();
      info.flow.dst_port = tcp_layer->getDstPort();
    }
  }
  if (parsedPacket.isPacketOfType(pcpp::UDP)) {
    pcpp::UdpLayer *udp_layer = parsedPacket.getLayerOfType<pcpp::UdpLayer>();
    if (udp_layer) {
      info.flow.src_port = udp_layer->getSrcPort();
      info.flow.dst_port = udp_layer->getDstPort();
    }
  }

  info.summary = info.protocol_name + " " + info.flow.src + ":" +
                 std::to_string(info.flow.src_port) + " -> " + info.flow.dst +
                 ":" + std::to_string(info.flow.dst_port);

  packet_queue_.Push(info);
}

} // namespace sniffles::capture
