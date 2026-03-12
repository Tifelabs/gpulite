#include "gpu_info.hpp"
#include "../utils/shell.hpp"
#include "../ui/color.hpp"
#include "../ui/terminal.hpp"
#include "../ui/banner.hpp"
#include <iostream>
#include <iomanip>

GPUInfo parseGPUInfo() {
  GPUInfo info;
  std::string raw = execCommand("system_profiler SPDisplaysDataType 2>/dev/null");

  auto extractField = [&](const std::string& key) -> std::string {
    size_t pos = raw.find(key);
    if (pos == std::string::npos) return "N/A";
    pos = raw.find(":", pos);
    if (pos == std::string::npos) return "N/A";
    pos++;
    size_t end = raw.find("\n", pos);
    std::string val = raw.substr(pos, end - pos);
    size_t s = val.find_first_not_of(" \t");
    size_t e = val.find_last_not_of(" \t\r");
    return (s == std::string::npos) ? "N/A" : val.substr(s, e - s + 1);
  };

  info.name         = extractField("Chipset Model");
  info.vendor       = extractField("Vendor");
  info.vram         = extractField("VRAM");
  info.metalSupport = extractField("Metal");
  info.coreCount    = extractField("Total Number of Cores");
  info.deviceID     = extractField("Device ID");

  return info;
}

void viewGPUStatus() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::GREEN << "  ● GPU STATUS\n" << Color::RESET;
  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Querying hardware...\n" << Color::RESET;
  std::cout.flush();

  GPUInfo gpu = parseGPUInfo();

  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::GREEN << "  ● GPU STATUS\n" << Color::RESET;
  Term::printLine('-', 60);

  auto row = [](const std::string& k, const std::string& v) {
    std::cout << "  " << Color::DIM << std::left << std::setw(20)
              << k << Color::RESET << Color::BOLD << v << Color::RESET << "\n";
  };

  std::cout << "\n";
  row("GPU Name",      gpu.name);
  row("Vendor",        gpu.vendor);
  row("VRAM",          gpu.vram);
  row("Metal Support", gpu.metalSupport);
  row("GPU Cores",     gpu.coreCount);
  row("Device ID",     gpu.deviceID);

  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Press ENTER to return...\n" << Color::RESET;
  std::cin.ignore();
  std::cin.get();
}