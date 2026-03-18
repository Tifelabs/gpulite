#include "benchmark.hpp"
#include "metal_bench.hpp"
#include "../ui/color.hpp"
#include "../ui/terminal.hpp"
#include "../ui/banner.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

void gpuBenchmark() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::YELLOW << "  ● GPU BENCHMARK\n" << Color::RESET;
  Term::printLine('-', 60);
  std::cout << "\n  " << Color::YELLOW << "Running Metal benchmark — please wait...\n\n" << Color::RESET;

  std::cout << "  " << Color::CYAN << "▶ " << Color::RESET
            << std::left << std::setw(26) << "Matrix Multiply (512x512)";
  std::cout.flush();

  MetalBenchResult res = runMetalBenchmark();

  if (!res.supported) {
    std::cout << Color::RED << "Metal not available on this device.\n" << Color::RESET;
    std::cin.ignore(); std::cin.get();
    return;
  }

  std::cout << Color::GREEN << Color::BOLD
            << std::fixed << std::setprecision(2)
            << res.gflops << " GFLOPS" << Color::RESET << "\n";

  std::cout << "  " << Color::CYAN << "▶ " << Color::RESET
            << std::left << std::setw(26) << "Memory Bandwidth (128MB)";
  std::cout << Color::GREEN << Color::BOLD
            << std::fixed << std::setprecision(2)
            << res.bandwidth_gb << " GB/s" << Color::RESET << "\n";

  float score = (res.gflops * 0.6f) + (res.bandwidth_gb * 0.4f);

  std::string tier = score > 3000 ? " ELITE       (M4 Pro / M5 class)"    :
                     score > 2000 ? "⚡ HIGH END   (M3 Pro / M4 base)"      :
                     score > 1000 ? "✔  MID RANGE (M2 Pro / M3 base)"      :
                     score > 400  ? "·  BASE      (M1 / M2 base)"           :
                                  "·  ENTRY       (older hardware)";

  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << "\n  " << Color::BOLD << Color::MAGENTA
            << "  COMPUTE:    " << res.gflops << " GFLOPS\n" << Color::RESET;
  std::cout << "  " << Color::BOLD << Color::CYAN
            << "  BANDWIDTH:  " << res.bandwidth_gb << " GB/s\n" << Color::RESET;
  std::cout << "  " << Color::BOLD << Color::GREEN
            << "  SCORE:      " << score << "\n" << Color::RESET;
  std::cout << "  " << Color::DIM << "  TIER:  " << Color::RESET
            << Color::BOLD << tier << Color::RESET << "\n\n";

  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Press ENTER to return...\n" << Color::RESET;
  std::cin.ignore();
  std::cin.get();
}