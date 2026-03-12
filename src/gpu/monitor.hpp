#pragma once
#include <csignal>

extern volatile sig_atomic_t liveRunning;
void handleSigInt(int);
void printProgressBar(const std::string& label, float pct, int width = 30);
void liveMonitor();