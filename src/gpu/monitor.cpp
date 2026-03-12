#include "monitor.hpp"
#include "../utils/shell.hpp"
#include "../ui/color.hpp"
#include "../ui/terminal.hpp"
#include "../ui/banner.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>
#include <ctime>

volatile sig_atomic_t liveRunning = 1;
void handleSigInt(int) { liveRunning = 0; }

void printProgressBar(const std::string& label, float pct, int width) {
  int filled = (int)(pct / 100.0f * width);
  std::cout << "  " << Color::CYAN << std::left << std::setw(18) << label
            << Color::RESET << " [";

  const char* barColor = pct > 80 ? Color::RED :
                         pct > 50 ? Color::YELLOW : Color::GREEN;

  std::cout << barColor;
  for (int i = 0; i < width; i++)
    std::cout << (i < filled ? "█" : "░");
  std::cout << Color::RESET << "] "
            << Color::BOLD << std::setw(5) << std::fixed
            << std::setprecision(1) << pct << "%"
            << Color::RESET << "\n";
}

void liveMonitor() {
  signal(SIGINT, handleSigInt);
  liveRunning = 1;
  Term::hideCursor();

  while (liveRunning) {
    Term::clearScreen();
    printBanner();
    std::cout << Color::BOLD << Color::CYAN
              << "  ● LIVE MONITOR   " << Color::DIM
              << "[Press Ctrl+C to exit]\n" << Color::RESET;
    Term::printLine('-', 60);

    std::string cpuRaw = execCommand("top -l 1 -n 0 | grep 'CPU usage' 2>/dev/null");
    float userPct = 0, sysPct = 0, idlePct = 100;
    sscanf(cpuRaw.c_str(), " CPU usage: %f%% user, %f%% sys, %f%% idle",
           &userPct, &sysPct, &idlePct);
    float gpuPct = userPct + sysPct;

    std::string memRaw = execCommand("vm_stat | grep 'Pages active' 2>/dev/null");
    long activePages = 0;
    sscanf(memRaw.c_str(), "Pages active: %ld.", &activePages);
    float memUsedGB = (activePages * 4096.0f) / (1024.0f * 1024.0f * 1024.0f);

    std::cout << "\n";
    printProgressBar("GPU Load",      gpuPct    > 100 ? 100 : gpuPct);
    printProgressBar("Memory Active", memUsedGB > 16  ? 100 : memUsedGB / 16 * 100);
    printProgressBar("CPU User",      userPct   > 100 ? 100 : userPct);
    printProgressBar("CPU System",    sysPct    > 100 ? 100 : sysPct);

    std::cout << "\n";
    Term::printLine('-', 60);

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << Color::DIM << "  Updated: " << std::ctime(&t) << Color::RESET;

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  Term::showCursor();
  signal(SIGINT, SIG_DFL);
}