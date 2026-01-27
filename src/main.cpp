#include <iostream>
#include <string>
#include <vector>

#include "capture/DeviceEnumerator.h"

namespace {
void PrintHelp(const char *program) {
  std::cout << "sniffles - packet sniffer CLI\n\n"
            << "Usage: " << program << " [--help] [--list-ifaces]\n\n"
            << "Options:\n"
            << "  --help        Show this help message\n"
            << "  --list-ifaces List capture interfaces\n"
            << "  --capture-iface Capture from a given interface \n";
}
} // namespace

int main(int argc, char *argv[]) {
  std::vector<std::string> args(argv + 1, argv + argc);

  for (const auto &arg : args) {
    if (arg == "--help" || arg == "-h") {
      PrintHelp(argv[0]);
      return 0;
    }
  }

  bool list_ifaces = false;
  bool capture_iface = false;
  std::string unknown_option;

  for (const auto &arg : args) {
    if (arg == "--list-ifaces") {
      list_ifaces = true;
    } else if (arg == "--capture-iface") {
      capture_iface = true;
      // parse the value of the device name as well
      std::cout << "capturing from interface" << "\n";
    }

    else if (arg == "--help" || arg == "-h") {
      continue;
    } else {
      unknown_option = arg;
      break;
    }
  }

  if (!unknown_option.empty()) {
    std::cerr << "Unknown option: " << unknown_option << "\n";
    PrintHelp(argv[0]);
    return 2;
  }

  if (list_ifaces) {
    std::vector<sniffles::capture::DeviceInfo> devices;
    std::string error;
    if (!sniffles::capture::DeviceEnumerator::ListDevices(devices, error)) {
      std::cerr << error << "\n";
      return 1;
    }

    for (const auto &device : devices) {
      std::cout << device.name << " - " << device.description << "\n";
    }
    return 0;
  }

  if (capture_iface) {
    // implemenet capture inface
  }

  PrintHelp(argv[0]);
  return 0;
}
