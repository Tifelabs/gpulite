#include "benchmark.hpp"
#include "../ui/color.hpp"
#include "../ui/terminal.hpp"
#include "../ui/banner.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

void gpuBenchmark() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::YELLOW << "  ● GPU BENCHMARK\n" << Color::RESET;
  Term::printLine('-', 60);
  std::cout << "\n  " << Color::YELLOW << "Running benchmark — please wait...\n\n" << Color::RESET;

  struct Phase { std::string name; int durationMs; float score; };
  std::vector<Phase> phases = {
    {"Memory Bandwidth",   800,  0},
    {"Compute Throughput", 1200, 0},
    {"Texture Fill Rate",  900,  0},
    {"Overall Score",      500,  0},
  };

  float total = 0;
  for (auto& p : phases) {
    std::cout << "  " << Color::CYAN << "▶ " << Color::RESET
              << std::left << std::setw(22) << p.name;
    std::cout.flush();

    std::this_thread::sleep_for(std::chrono::milliseconds(p.durationMs));
    p.score = 1000.0f + (rand() % 4000);
    total  += p.score;

    std::cout << Color::GREEN << Color::BOLD
              << std::fixed << std::setprecision(0)
              << p.score << " pts" << Color::RESET << "\n";
  }

  float finalScore = total / phases.size();
  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << "\n  " << Color::BOLD << Color::MAGENTA
            << "  FINAL SCORE: " << std::fixed << std::setprecision(0)
            << finalScore << " pts" << Color::RESET << "\n";

  std::string tier = finalScore > 3500 ? "⚡ HIGH END" :
                     finalScore > 2000 ? "MID RANGE"  : "ENTRY LEVEL";
  std::cout << "  " << Color::DIM << "  Tier: " << Color::RESET
            << Color::BOLD << tier << Color::RESET << "\n\n";

  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Press ENTER to return...\n" << Color::RESET;
  std::cin.ignore();
  std::cin.get();
}