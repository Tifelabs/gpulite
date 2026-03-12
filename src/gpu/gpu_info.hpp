#pragma once
#include <string>

struct GPUInfo {
  std::string name;
  std::string vendor;
  std::string vram;
  std::string metalSupport;
  std::string coreCount;
  std::string deviceID;
};

GPUInfo parseGPUInfo();
void viewGPUStatus();