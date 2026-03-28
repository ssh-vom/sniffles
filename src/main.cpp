#include <boost/program_options.hpp>
#include <iostream>
#include <string>

#include "app/App.h"
#include "capture/CaptureService.h"
#include "capture/DeviceEnumerator.h"

namespace po = boost::program_options;

namespace {
void PrintHelp(const po::options_description &desc) {
  std::cout << "sniffles - packet sniffer CLI\n\n" << desc << "\n";
}
} // namespace

int main(int argc, char *argv[]) {
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "Show this help message")
    ("list-ifaces", "List capture interfaces")
    ("capture-iface", po::value<std::string>(), "Capture from a given interface")
    ("filter", po::value<std::string>(), "Apply a BPF filter to capture")
    ("config-file", po::value<std::string>(), "Load options from a configuration file");

  po::variables_map vm;

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("config-file")) {
      std::string config_file = vm["config-file"].as<std::string>();
      po::store(po::parse_config_file<char>(config_file.c_str(), desc), vm);
    }

    po::notify(vm);
  } catch (const po::error &e) {
    std::cerr << "Error: " << e.what() << "\n";
    PrintHelp(desc);
    return 2;
  }

  if (vm.count("help")) {
    PrintHelp(desc);
    return 0;
  }

  if (vm.empty() || (!vm.count("list-ifaces") && !vm.count("capture-iface"))) {
    sniffles::app::App app;
    return app.Run(argc, argv);
  }

  if (vm.count("list-ifaces")) {
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

  if (vm.count("capture-iface")) {
    std::string interface = vm["capture-iface"].as<std::string>();
    std::string filter_expression;

    if (vm.count("filter")) {
      filter_expression = vm["filter"].as<std::string>();
    }

    sniffles::capture::CaptureService capture_service;
    sniffles::capture::CaptureRequest request;
    request.device_name = interface;
    request.filter_expression = filter_expression;

    if (!capture_service.Start(request)) {
      std::cerr << "Failed to start capture on " << interface << "\n";
      return 1;
    }

    std::cout << "Capturing on " << interface << "... Press Enter to stop.\n";
    std::cin.get();
    capture_service.Stop();
    return 0;
  }

  PrintHelp(desc);
  return 0;
}
