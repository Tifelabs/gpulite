#include "shell.hpp"
#include <cstdio>

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