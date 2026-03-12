#include "process.hpp"
#include "../utils/shell.hpp"
#include "../ui/color.hpp"
#include "../ui/terminal.hpp"
#include "../ui/banner.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

void gpuProcess() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::BLUE << "  ● GPU PROCESSES\n" << Color::RESET;
  Term::printLine('-', 60);

  std::string raw = execCommand(
    "ps aux | grep -E 'WindowServer|MTLCompiler|GPU|Metal|coremedia|CoreDisplay' "
    "| grep -v grep | awk '{print $2, $3, $4, $11}' 2>/dev/null"
  );

  std::cout << "\n";
  std::cout << "  " << Color::DIM
            << std::left << std::setw(8) << "PID"
            << std::setw(8) << "%CPU"
            << std::setw(8) << "%MEM"
            << "PROCESS\n" << Color::RESET;
  Term::printLine(' ', 60);

  if (raw.empty()) {
    std::cout << "  " << Color::DIM << "No GPU processes detected.\n" << Color::RESET;
  } else {
    std::istringstream ss(raw);
    std::string line;
    int count = 0;
    while (std::getline(ss, line) && count < 15) {
      std::istringstream ls(line);
      std::string pid, cpu, mem, proc;
      ls >> pid >> cpu >> mem >> proc;

      size_t slash = proc.rfind('/');
      if (slash != std::string::npos) proc = proc.substr(slash + 1);

      std::cout << "  "
                << Color::CYAN   << std::left << std::setw(8) << pid  << Color::RESET
                << Color::YELLOW << std::setw(8) << cpu  << Color::RESET
                << Color::GREEN  << std::setw(8) << mem  << Color::RESET
                << proc << "\n";
      count++;
    }
  }

  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Press ENTER to return...\n" << Color::RESET;
  std::cin.ignore();
  std::cin.get();
}