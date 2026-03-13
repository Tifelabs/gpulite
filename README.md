# gpulite
> Lightweight GPU monitor & benchmark tool for macOS — built in C++

![Platform](https://img.shields.io/badge/platform-macOS-lightgrey)
![Language](https://img.shields.io/badge/language-C%2B%2B17-blue)
![License](https://img.shields.io/badge/license-GNU-green)
![Version](https://img.shields.io/badge/version-0.1.0-orange)

## Features
- **GPU Status** — Hardware overview: name, vendor, VRAM, Metal support, core count
- **GPU Benchmark** — Stress test with scored phases and tier rating
- **GPU Processes** — Active GPU-adjacent workloads with CPU/MEM usage
- **Live Monitor** — Real-time updating stats with colored progress bars

## Building with CMake

```bash
git clone https://github.com/tifelabs/gpulite
cd gpulite
mkdir build && cd build
cmake ..
make
./gpulite
```

#### OR

```bash
git clone https://github.com/tifelabs/gpulite
cd gpulite
cmake -S . -B build && cmake --build build
./gpulite
```


Or directly with g++:

```bash
g++ -std=c++17 -O2 -o gpulite main.cpp src/**/*.cpp && ./gpulite
```

## wip
> WIP — balancing this project alongside school

- [x] GPU status (Metal device info)
- [x] GPU benchmark (scored phases)
- [x] GPU process viewer
- [x] Live real-time monitor
- [ ] Metal compute benchmark (true GPU stress test)
- [ ] Export results to JSON/CSV
- [ ] Linux support (NVML / ROCm)
- [ ] Windows support

## Requirements
- macOS 10.14+
- C++17 compiler (Xcode CLT or Homebrew LLVM)
- CMake 3.16+ (optional)

## License
GNU General Public License v3.0