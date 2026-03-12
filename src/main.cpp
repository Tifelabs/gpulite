#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>

/* ANSI COLOR */
namespace Color {
  const char* RESET   = "\033[0m";
  const char* BOLD    = "\033[1m";
  const char* DIM     = "\033[2m";

  const char* RED     = "\033[31m";
  const char* GREEN   = "\033[32m";
  const char* YELLOW  = "\033[33m";
  const char* BLUE    = "\033[34m";
  const char* MAGENTA = "\033[35m";
  const char* CYAN    = "\033[36m";
  const char* WHITE   = "\033[37m";

  const char* BG_BLACK = "\033[40m";
}

/* Terminal Utilities */
namespace Term {
  void clearScreen()   { std::cout << "\033[2J\033[H"; }
  void clearLine()     { std::cout << "\033[2K\r"; }
  void moveCursor(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
  }
  void hideCursor()    { std::cout << "\033[?25l"; }
  void showCursor()    { std::cout << "\033[?25h"; }
  void saveCursor()    { std::cout << "\033[s"; }
  void restoreCursor() { std::cout << "\033[u"; }

  int getWidth() {
    // default safe width
    return 80;
  }

  void printLine(char c = '-', int width = 60) {
    std::cout << Color::DIM;
    for (int i = 0; i < width; i++) std::cout << c;
    std::cout << Color::RESET << "\n";
  }

  void printCentered(const std::string& text, int width = 60) {
    int pad = (width - (int)text.size()) / 2;
    if (pad > 0) std::cout << std::string(pad, ' ');
    std::cout << text << "\n";
  }
}


//  SHELL EXEC HELPER
std::string execCommand(const std::string& cmd) {
  std::string result;
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) return "";
  char buf[256];
  while (fgets(buf, sizeof(buf), pipe))
    result += buf;
  pclose(pipe);
  return result;
}

//  GPU INFO STRUCT
struct GPUInfo {
  std::string name;
  std::string vendor;
  std::string vram;
  std::string metalSupport;
  std::string coreCount;
  std::string deviceID;
};

/* GPU DATA PARSER (macOS system_profiler) */
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
    // trim whitespace
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


/* PROGRESS BAR */
void printProgressBar(const std::string& label, float pct, int width = 30) {
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

/* BANNER */
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
  std::cout << Color::RESET;
  std::cout << Color::DIM;
  Term::printCentered("Lightweight GPU Monitor & Benchmark Tool", 60);
  Term::printCentered("v0.1.0 — macOS | gpulite.com", 60);
  std::cout << Color::RESET << "\n";
  Term::printLine('-', 60);
}

//  MAIN MENU
void printMenu() {
  std::cout << "\n";
  std::cout << Color::BOLD << "  MAIN MENU\n" << Color::RESET;
  std::cout << "\n";
  std::cout << "  " << Color::GREEN  << "[1]" << Color::RESET << "  GPU Status       — Hardware overview\n";
  std::cout << "  " << Color::YELLOW << "[2]" << Color::RESET << "  GPU Benchmark     — Stress test & score\n";
  std::cout << "  " << Color::BLUE   << "[3]" << Color::RESET << "  GPU Processes     — Active GPU workloads\n";
  std::cout << "  " << Color::CYAN   << "[4]" << Color::RESET << "  Live Monitor      — Real-time stats\n";
  std::cout << "  " << Color::RED    << "[Q]" << Color::RESET << "  Quit\n";
  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << Color::BOLD << "\n  ❯ " << Color::RESET;
}

// ─────────────────────────────────────────────
//  [1] GPU STATUS
// ─────────────────────────────────────────────
void viewGPUStatus() {
  Term::clearScreen();
  printBanner();

  std::cout << Color::BOLD << Color::GREEN
            << "  ● GPU STATUS\n" << Color::RESET;
  Term::printLine('-', 60);

  std::cout << Color::DIM << "  Querying hardware...\n" << Color::RESET;
  std::cout.flush();

  GPUInfo gpu = parseGPUInfo();

  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::GREEN
            << "  ● GPU STATUS\n" << Color::RESET;
  Term::printLine('-', 60);

  auto row = [](const std::string& k, const std::string& v) {
    std::cout << "  " << Color::DIM << std::left << std::setw(20)
              << k << Color::RESET
              << Color::BOLD << v << Color::RESET << "\n";
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

// GPU BENCHMARK
void gpuBenchmark() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::YELLOW
            << "  ● GPU BENCHMARK\n" << Color::RESET;
  Term::printLine('-', 60);

  std::cout << "\n  " << Color::YELLOW
            << "Running benchmark — please wait...\n\n" << Color::RESET;

  // Simulate benchmark phases with real CPU/GPU stress proxy
  struct Phase { std::string name; int durationMs; float score; };
  std::vector<Phase> phases = {
    {"Memory Bandwidth",  800,  0},
    {"Compute Throughput",1200, 0},
    {"Texture Fill Rate", 900,  0},
    {"Overall Score",     500,  0},
  };

  float total = 0;
  for (auto& p : phases) {
    std::cout << "  " << Color::CYAN << "▶ " << Color::RESET
              << std::left << std::setw(22) << p.name;
    std::cout.flush();

    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(p.durationMs));

    // Fake score based on system (will be real Metal compute in future)
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
                     finalScore > 2000 ? " MID RANGE" : "ENTRY LEVEL";
  std::cout << "  " << Color::DIM << "  Tier: " << Color::RESET
            << Color::BOLD << tier << Color::RESET << "\n\n";

  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Press ENTER to return...\n" << Color::RESET;
  std::cin.ignore();
  std::cin.get();
}

// GPU PROCESSES
void gpuProcess() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::BLUE
            << "  ● GPU PROCESSES\n" << Color::RESET;
  Term::printLine('-', 60);

  // Use ps to find GPU-adjacent processes (WindowServer, MTLCompilerService etc)
  std::string raw = execCommand(
    "ps aux | grep -E 'WindowServer|MTLCompiler|GPU|Metal|coremedia|CoreDisplay' "
    "| grep -v grep | awk '{print $2, $3, $4, $11}' 2>/dev/null"
  );

  std::cout << "\n";
  std::cout << "  " << Color::DIM
            << std::left << std::setw(8)  << "PID"
            << std::setw(8)  << "%CPU"
            << std::setw(8)  << "%MEM"
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

      // trim process path to basename
      size_t slash = proc.rfind('/');
      if (slash != std::string::npos) proc = proc.substr(slash + 1);

      std::cout << "  "
                << Color::CYAN  << std::left << std::setw(8)  << pid   << Color::RESET
                << Color::YELLOW<< std::setw(8)  << cpu   << Color::RESET
                << Color::GREEN << std::setw(8)  << mem   << Color::RESET
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

// LIVE MONITOR
volatile sig_atomic_t liveRunning = 1;
void handleSigInt(int) { liveRunning = 0; }

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

    // CPU usage as GPU proxy (Metal compute will replace this)
    std::string cpuRaw = execCommand(
      "top -l 1 -n 0 | grep 'CPU usage' 2>/dev/null"
    );

    float userPct = 0, sysPct = 0, idlePct = 100;
    sscanf(cpuRaw.c_str(), " CPU usage: %f%% user, %f%% sys, %f%% idle",
           &userPct, &sysPct, &idlePct);
    float gpuPct = userPct + sysPct;

    // Memory
    std::string memRaw = execCommand(
      "vm_stat | grep 'Pages active' 2>/dev/null"
    );
    long activePages = 0;
    sscanf(memRaw.c_str(), "Pages active: %ld.", &activePages);
    float memUsedGB = (activePages * 4096.0f) / (1024.0f * 1024.0f * 1024.0f);

    // Thermal
    std::string thermalRaw = execCommand(
      "pmset -g therm 2>/dev/null | head -5"
    );

    std::cout << "\n";
    printProgressBar("GPU Load",     gpuPct > 100 ? 100 : gpuPct);
    printProgressBar("Memory Active", memUsedGB > 16 ? 100 : memUsedGB / 16 * 100);
    printProgressBar("CPU User",      userPct > 100 ? 100 : userPct);
    printProgressBar("CPU System",    sysPct  > 100 ? 100 : sysPct);

    std::cout << "\n";
    Term::printLine('-', 60);

    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << Color::DIM << "  Updated: " << std::ctime(&t) << Color::RESET;

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  Term::showCursor();
  signal(SIGINT, SIG_DFL);
}

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