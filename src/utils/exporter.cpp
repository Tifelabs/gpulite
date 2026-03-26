#include "exporter.hpp"
#include "../ui/color.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

// ── HELPERS ──
static std::string currentTimestamp() {
  std::time_t now = std::time(nullptr);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", std::localtime(&now));
  return std::string(buf);
}

static std::string filenameTimestamp() {
  std::time_t now = std::time(nullptr);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&now));
  return std::string(buf);
}

static bool writeJSON(const ExportData& d, const std::string& path) {
  std::ofstream f(path);
  if (!f.is_open()) return false;

  f << "{\n";
  f << "  \"tool\": \"gpulite\",\n";
  f << "  \"version\": \"0.1.0\",\n";
  f << "  \"timestamp\": \"" << d.timestamp << "\",\n";
  f << "  \"gpu\": \"" << d.gpuName << "\",\n";
  f << "  \"benchmark\": {\n";
  f << "    \"compute_gflops\": " << std::fixed << std::setprecision(2) << d.gflops << ",\n";
  f << "    \"bandwidth_gbs\": " << std::fixed << std::setprecision(2) << d.bandwidth_gb << ",\n";
  f << "    \"score\": " << std::fixed << std::setprecision(1) << d.score << ",\n";
  f << "    \"tier\": \"" << d.tier << "\"\n";
  f << "  }\n";
  f << "}\n";

  return f.good();
}

static bool writeCSV(const ExportData& d, const std::string& path) {
  std::ifstream check(path);
  bool exists = check.good();
  check.close();

  std::ofstream f(path, std::ios::app);
  if (!f.is_open()) return false;

  // Write header only if file is new
  if (!exists)
    f << "timestamp,gpu,compute_gflops,bandwidth_gbs,score,tier\n";

  f << d.timestamp << ","
    << d.gpuName   << ","
    << std::fixed  << std::setprecision(2) << d.gflops << ","
    << std::fixed  << std::setprecision(2) << d.bandwidth_gb << ","
    << std::fixed  << std::setprecision(1) << d.score << ","
    << d.tier      << "\n";

  return f.good();
}

// ── MAIN EXPORT FUNCTION ──
void exportResults(const ExportData& data) {
  std::string ts       = filenameTimestamp();
  std::string jsonPath = "gpulite_" + ts + ".json";
  std::string csvPath  = "gpulite_results.csv";  // CSV appends, so one file

  std::cout << "\n";

  // JSON
  bool jsonOk = writeJSON(data, jsonPath);
  std::cout << "  " << (jsonOk ? Color::GREEN : Color::RED)
            << (jsonOk ? "✔" : "✘") << Color::RESET
            << "  JSON  → " << jsonPath << "\n";

  // CSV
  bool csvOk = writeCSV(data, csvPath);
  std::cout << "  " << (csvOk ? Color::GREEN : Color::RED)
            << (csvOk ? "✔" : "✘") << Color::RESET
            << "  CSV   → " << csvPath
            << Color::DIM << " (appended)" << Color::RESET << "\n";
}