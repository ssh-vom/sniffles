#include <iostream>
#include <string>
#include <vector>

#include "app/App.h"
#include "capture/CaptureService.h"
#include "capture/DeviceEnumerator.h"

namespace {
void PrintHelp(const char *program) {
  std::cout << "sniffles - packet sniffer CLI\n\n"
            << "Usage: " << program
            << " [--help] [--list-ifaces] [--capture-iface <name>] [--filter <expr>]\n\n"
            << "Options:\n"
            << "  --help        Show this help message\n"
            << "  --list-ifaces List capture interfaces\n"
            << "  --capture-iface Capture from a given interface\n"
            << "  --filter      Apply a BPF filter to capture\n"
            << "\nRun with no options to open the UI.\n";
}
} // namespace

int main(int argc, char *argv[]) {

  std::vector<std::string> args(argv + 1, argv + argc);
  std::string interface;
  bool list_ifaces = false;
  bool capture_iface = false;
  std::string filter_expression;
  std::string unknown_option;

  for (size_t i = 0; i < args.size(); ++i) {
    const auto &arg = args[i];
    if (arg == "--list-ifaces") {
      list_ifaces = true;
    } else if (arg == "--capture-iface") {
      capture_iface = true;
      if (i + 1 >= args.size()) {
        std::cerr << "Error: --capture-iface requires an interface name \n";
        return 1;
      }
      interface = args[i + 1];
      ++i;
    } else if (arg == "--filter") {
      if (i + 1 >= args.size()) {
        std::cerr << "Error: --filter requires an expression\n";
        return 1;
      }
      filter_expression = args[i + 1];
      ++i;
    } else if (arg == "--help" || arg == "-h") {
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

  if (args.empty()) {
    sniffles::app::App app;
    return app.Run(argc, argv);
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
    sniffles::capture::CaptureService capture_service;
    sniffles::capture::CaptureRequest request;
    request.device_name = interface;
    request.filter_expression = filter_expression;
    if (!capture_service.Start(request)) {
      std::cerr << "Failed to start capture on " << interface << "\n";
      return 1;
    }

    std::cout << "Capturing on " << interface << "... Press Enter to stop .\n";
    std::cin.get();
    capture_service.Stop();
    return 0;
  }

  PrintHelp(argv[0]);
  return 0;
}
