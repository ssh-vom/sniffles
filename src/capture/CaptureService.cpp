#include "capture/CaptureService.h"
#include <IPv4Layer.h>
#include <Packet.h>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <RawPacket.h>
#include <iostream>
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

  //   // handle packet should just print out the packet data for now
  //
  //   const timespec timestamp = packet->getPacketTimeStamp();
  //   const int frame_length = packet->getFrameLength();
  //   const pcpp::LinkLayerType link_layer_type = packet->getLinkLayerType();
  //   const uint8_t *raw_data = packet->getRawData();
  //   // const int raw_data_len = packet->getRawDataLen();
  //   std::cout << "Time:" << timestamp.tv_sec << "." << timestamp.tv_nsec /
  //   1000
  //             << " | Length: " << "frame length" << frame_length << "raw
  //             data"
  //             << &raw_data << "link layer type" << link_layer_type << "\n";

  pcpp::Packet parsedPacket(packet);
  std::cout << "Packet Length: " << parsedPacket.getRawPacket()->getRawDataLen()
            << " bytes\n";

  if (parsedPacket.isPacketOfType(pcpp::IPv4)) {
    pcpp::IPv4Layer *ip_layer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
    if (ip_layer) {
      std::cout << " Source IP: " << ip_layer->getSrcIPAddress() << "\n";
      std::cout << " Dest IP: " << ip_layer->getDstIPAddress() << "\n";
      std::cout << " Protocol: " << GetProtocolName(ip_layer->getProtocol())
                << "\n";
    }
  }
}

} // namespace sniffles::capture
