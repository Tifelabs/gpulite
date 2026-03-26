#pragma once
#include "../gpu/metal_bench.hpp"
#include <string>

struct ExportData {
  float       gflops;
  float       bandwidth_gb;
  float       score;
  std::string tier;
  std::string timestamp;
  std::string gpuName;
};

void exportResults(const ExportData& data);