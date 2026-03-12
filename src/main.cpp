#include <iostream>
#include <unistd.h>


void options(){
    std::cout << "\t\t SELECT GPU OPTION\n";
    std::cout << R"(
                    "[1] ::VIEW GPU STATUS"
                    "[2] ::GPU BENCHMARK"
                    "[3] ::GPU PROCESS"
                    "[Q] ::QUIT"
    )" << "\n";
}

void gpuBenchmark() {
  std::cout << "\n--- GPU BENCHMARK --- (coming soon)\n";
}

void gpuProcess() {
  std::cout << "\n--- GPU PROCESSES --- (coming soon)\n";
  int new_process = fork();
  if(new_process < 0){
    std::cerr << "Fork Failed\n";
    exit(1);
  }
  if(new_process == 0){
    std::cout << "NEW PROCESS -> " << getpid() << "\n";
    exit(0);
  }

  std::cout << "PARENT PROCESS -> " << getpid() << "\n";
}

int main(int argc, char *argv[], char **envp) {
  char choice;

  while (true) {
    options();
    std::cin >> choice;

    switch (choice) {
      case '1': viewGPUStatus(); break;
      case '2': gpuBenchmark();  break;
      case '3': gpuProcess();    break;
      case 'Q':
      case 'q':
        std::cout << "\nExiting gpulite...\n";
        return 0;
      default:
        std::cout << "\n  Invalid option. Try again.\n";
    }

    std::cout << "\n";
  }

  return 0;
}
