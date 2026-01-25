#include "PcapLiveDeviceList.h"
#include <LiveCapture.hpp>
#include <iostream>
#include <stdlib.h>

// Pcap Live Device

using namespace pcpp;

void DisplayAdapterNames() {

  // Grab Interface Device Names
  PcapLiveDeviceList &deviceList = PcapLiveDeviceList::getInstance();

  std::vector<PcapLiveDevice *> devices = deviceList.getPcapLiveDevicesList();

  for (PcapLiveDevice *dev : devices) {

    std::cout << dev->getName() << "\n";
  }
}
