#include "terminal.hpp"
#include "color.hpp"
#include <iostream>

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

  int getWidth() { return 80; }

  void printLine(char c, int width) {
    std::cout << Color::DIM;
    for (int i = 0; i < width; i++) std::cout << c;
    std::cout << Color::RESET << "\n";
  }

  void printCentered(const std::string& text, int width) {
    int pad = (width - (int)text.size()) / 2;
    if (pad > 0) std::cout << std::string(pad, ' ');
    std::cout << text << "\n";
  }

}