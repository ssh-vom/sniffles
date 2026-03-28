#include "capture/CaptureService.h"

#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IPv6Layer.h>
#include <Packet.h>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <RawPacket.h>
#include <TcpLayer.h>
#include <UdpLayer.h>

#include <cassert>
#include <chrono>

namespace {
std::string BuildSummary(const sniffles::decode::PacketInfo &info) {
  if (info.flow.src.empty() || info.flow.dst.empty()) {
    return info.protocol_name + " frame";
  }

  if (info.flow.src_port == 0 && info.flow.dst_port == 0) {
    return info.protocol_name + " " + info.flow.src + " -> " + info.flow.dst;
  }

  return info.protocol_name + " " + info.flow.src + ":" +
         std::to_string(info.flow.src_port) + " -> " + info.flow.dst + ":" +
         std::to_string(info.flow.dst_port);
}
} // namespace

namespace sniffles::capture {
bool CaptureService::Start(const CaptureRequest &request) {
  assert(!request.device_name.empty());
  device_name_ = request.device_name;
  filter_expression_ = request.filter_expression;

  pcpp::PcapLiveDeviceList &device_list =
      pcpp::PcapLiveDeviceList::getInstance();
  device_ = device_list.getDeviceByName(device_name_);

  if (device_ == nullptr)
    return false;

  if (!device_->open()) { // if we can't open the device
    device_ = nullptr;
    return false; // failed
  }

  if (!filter_expression_.empty() && !device_->setFilter(filter_expression_)) {
    device_->close();
    device_ = nullptr;
    return false;
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
CaptureStats CaptureService::GetStats() const {
  if (!running_) {
    return {};
  }

  pcpp::IPcapDevice::PcapStats stats;
  device_->getStatistics(stats);
  return {stats.packetsRecv, stats.packetsDrop, stats.packetsDropByInterface};
}

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
  info.raw_data.assign(packet->getRawData(),
                       packet->getRawData() + packet->getRawDataLen());
  info.protocol_name = "Ethernet";

  pcpp::Packet parsedPacket(packet);

  if (parsedPacket.isPacketOfType(pcpp::Ethernet)) {
    pcpp::EthLayer *ethernet_layer = parsedPacket.getLayerOfType<pcpp::EthLayer>();
    info.flow.src = ethernet_layer->getSourceMac().toString();
    info.flow.dst = ethernet_layer->getDestMac().toString();
  }

  if (parsedPacket.isPacketOfType(pcpp::IPv4)) {
    pcpp::IPv4Layer *ip_layer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
    if (ip_layer) {
      info.flow.src = ip_layer->getSrcIPAddress().toString();
      info.flow.dst = ip_layer->getDstIPAddress().toString();
      info.protocol_num = ip_layer->getIPv4Header()->protocol;
      info.protocol_name = GetProtocolName(info.protocol_num);
    }
  }
  if (parsedPacket.isPacketOfType(pcpp::IPv6)) {
    pcpp::IPv6Layer *ip_layer = parsedPacket.getLayerOfType<pcpp::IPv6Layer>();
    if (ip_layer) {
      info.flow.src = ip_layer->getSrcIPAddress().toString();
      info.flow.dst = ip_layer->getDstIPAddress().toString();
      info.protocol_num = ip_layer->getIPv6Header()->nextHeader;
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

  info.summary = BuildSummary(info);

  packet_queue_.Push(info);
}

} // namespace sniffles::capture
