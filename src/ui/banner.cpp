#include "banner.hpp"
#include "color.hpp"
#include "terminal.hpp"
#include <iostream>

void printBanner() {
  Term::clearScreen();
  std::cout << "\n";
  std::cout << Color::CYAN << Color::BOLD;
  std::cout << "  ██████╗ ██████╗ ██╗   ██╗██╗     ██╗████████╗███████╗\n";
  std::cout << "  ██╔════╝ ██╔══██╗██║   ██║██║     ██║╚══██╔══╝██╔════╝\n";
  std::cout << "  ██║  ███╗██████╔╝██║   ██║██║     ██║   ██║   █████╗  \n";
  std::cout << "  ██║   ██║██╔═══╝ ██║   ██║██║     ██║   ██║   ██╔══╝  \n";
  std::cout << "  ╚██████╔╝██║     ╚██████╔╝███████╗██║   ██║   ███████╗\n";
  std::cout << "   ╚═════╝ ╚═╝      ╚═════╝ ╚══════╝╚═╝   ╚═╝   ╚══════╝\n";
  std::cout << Color::RESET << Color::DIM;
  Term::printCentered("Lightweight GPU Monitor & Benchmark Tool", 60);
  Term::printCentered("v0.1.0 — macOS | gpulite.com", 60);
  std::cout << Color::RESET << "\n";
  Term::printLine('-', 60);
}

void printMenu() {
  std::cout << "\n" << Color::BOLD << "  MAIN MENU\n" << Color::RESET << "\n";
  std::cout << "  " << Color::GREEN  << "[1]" << Color::RESET << "  GPU Status       — Hardware overview\n";
  std::cout << "  " << Color::YELLOW << "[2]" << Color::RESET << "  GPU Benchmark     — Stress test & score\n";
  std::cout << "  " << Color::BLUE   << "[3]" << Color::RESET << "  GPU Processes     — Active GPU workloads\n";
  std::cout << "  " << Color::CYAN   << "[4]" << Color::RESET << "  Live Monitor      — Real-time stats\n";
  std::cout << "  " << Color::RED    << "[Q]" << Color::RESET << "  Quit\n";
  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << Color::BOLD << "\n  ❯ " << Color::RESET;
}