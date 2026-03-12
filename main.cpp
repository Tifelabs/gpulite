#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include "src/ui/color.hpp"
#include "src/ui/terminal.hpp"
#include "src/ui/banner.hpp"
#include "src/gpu/gpu_info.hpp"
#include "src/gpu/benchmark.hpp"
#include "src/gpu/process.hpp"
#include "src/gpu/monitor.hpp"

int main(int argc, char* argv[], char** envp) {
  srand(static_cast<unsigned>(time(nullptr)));

  char choice;

  while (true) {
    printBanner();
    printMenu();

    std::cin >> choice;

    switch (choice) {
      case '1': viewGPUStatus(); break;
      case '2': gpuBenchmark();  break;
      case '3': gpuProcess();    break;
      case '4': liveMonitor();   break;
      case 'Q':
      case 'q':
        Term::clearScreen();
        std::cout << "\n  " << Color::CYAN << Color::BOLD
                  << "gpulite — goodbye.\n\n" << Color::RESET;
        return 0;
      default:
        std::cout << "\n  " << Color::RED
                  << "Invalid option.\n" << Color::RESET;
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
  }

  return 0;
}