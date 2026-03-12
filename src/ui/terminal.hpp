#pragma once
#include <string>

namespace Term {
  void clearScreen();
  void clearLine();
  void moveCursor(int row, int col);
  void hideCursor();
  void showCursor();
  void saveCursor();
  void restoreCursor();
  int  getWidth();
  void printLine(char c = '-', int width = 60);
  void printCentered(const std::string& text, int width = 60);
}