#include "process.hpp"
#include "../ui/color.hpp"
#include "../ui/terminal.hpp"
#include "../ui/banner.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

#include <libproc.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <mach/mach.h>
#include <unistd.h>

struct ProcessInfo {
  pid_t       pid;
  std::string name;
  float       cpuTime;
  float       memMB;
};

static std::vector<pid_t> getRealGPUPIDs() {
  std::vector<pid_t> result;

  CFMutableDictionaryRef match = IOServiceMatching("IOAccelerator");
  io_iterator_t iter;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  kern_return_t kr = IOServiceGetMatchingServices(kIOMasterPortDefault, match, &iter);
#pragma clang diagnostic pop

  if (kr != KERN_SUCCESS) return result;

  io_service_t service;
  while ((service = IOIteratorNext(iter)) != IO_OBJECT_NULL) {
    CFTypeRef pidRef = IORegistryEntryCreateCFProperty(
      service,
      CFSTR("IOUserClientCreatorPID"),
      kCFAllocatorDefault,
      0
    );
    if (pidRef && CFGetTypeID(pidRef) == CFNumberGetTypeID()) {
      int32_t pid = 0;
      CFNumberGetValue((CFNumberRef)pidRef, kCFNumberSInt32Type, &pid);
      if (pid > 0) result.push_back((pid_t)pid);
      CFRelease(pidRef);
    }
    IOObjectRelease(service);
  }
  IOObjectRelease(iter);

  // Deduplicate
  std::sort(result.begin(), result.end());
  result.erase(std::unique(result.begin(), result.end()), result.end());

  return result;
}

static std::string getProcessName(pid_t pid) {
  char name[PROC_PIDPATHINFO_MAXSIZE];
  if (proc_name(pid, name, sizeof(name)) > 0)
    return std::string(name);
  return "unknown";
}

static bool getProcessStats(pid_t pid, float& cpuTime, float& memMB) {
  struct proc_taskinfo ti;
  int ret = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &ti, sizeof(ti));
  if (ret <= 0) return false;
  memMB   = (float)ti.pti_resident_size / (1024.0f * 1024.0f);
  cpuTime = (float)(ti.pti_total_user + ti.pti_total_system) / 1e9f;
  return true;
}

void gpuProcess() {
  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::BLUE
            << "  ● GPU PROCESSES\n" << Color::RESET;
  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Querying IOKit for GPU clients...\n" << Color::RESET;
  std::cout.flush();

  std::vector<pid_t> pids = getRealGPUPIDs();
  std::vector<ProcessInfo> procs;

  for (pid_t pid : pids) {
    std::string name = getProcessName(pid);
    size_t slash = name.rfind('/');
    if (slash != std::string::npos) name = name.substr(slash + 1);

    float cpu = 0, mem = 0;
    if (!getProcessStats(pid, cpu, mem)) continue;

    procs.push_back({pid, name, cpu, mem});
  }

  std::sort(procs.begin(), procs.end(),
    [](const ProcessInfo& a, const ProcessInfo& b) {
      return a.memMB > b.memMB;
    });

  Term::clearScreen();
  printBanner();
  std::cout << Color::BOLD << Color::BLUE
            << "  ● GPU PROCESSES\n" << Color::RESET;
  Term::printLine('-', 60);

  std::cout << "\n  " << Color::DIM
            << std::left << std::setw(8)  << "PID"
            << std::setw(28) << "PROCESS"
            << std::setw(12) << "MEM (MB)"
            << "CPU TIME(s)" << Color::RESET << "\n";
  Term::printLine('-', 60);

  if (procs.empty()) {
    std::cout << "\n  " << Color::DIM
              << "No active GPU clients found.\n" << Color::RESET;
  } else {
    int shown = 0;
    for (auto& p : procs) {
      if (shown++ >= 15) break;

      const char* memColor = p.memMB > 500 ? Color::RED :
                             p.memMB > 100 ? Color::YELLOW : Color::GREEN;

      std::string displayName = p.name.size() > 26
                                ? p.name.substr(0, 25) + "~"
                                : p.name;

      std::cout << "  "
                << Color::CYAN  << std::left << std::setw(8)  << p.pid     << Color::RESET
                << std::setw(28) << displayName
                << memColor     << std::fixed << std::setprecision(1)
                << std::setw(12) << p.memMB   << Color::RESET
                << Color::DIM   << std::setprecision(2) << p.cpuTime
                << Color::RESET << "\n";
    }
    std::cout << "\n  " << Color::DIM
              << "Total active GPU clients: " << procs.size()
              << Color::RESET << "\n";
  }

  std::cout << "\n";
  Term::printLine('-', 60);
  std::cout << Color::DIM << "  Press ENTER to return...\n" << Color::RESET;
  std::cin.ignore();
  std::cin.get();
}