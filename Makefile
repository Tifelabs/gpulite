CXX      = g++
OBJCXX   = clang++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc
MMFLAGS  = -std=c++17 -O2 -Wall -Isrc -fobjc-arc
LDFLAGS  = -framework Metal -framework Foundation -framework IOKit

TARGET   = gpulite
BUILDDIR = build/obj

# SOURCES 
CPP_SRCS = main.cpp \
           src/ui/terminal.cpp \
           src/ui/banner.cpp \
           src/utils/shell.cpp \
           src/gpu/gpu_info.cpp \
           src/gpu/benchmark.cpp \
           src/gpu/process.cpp \
           src/gpu/monitor.cpp

MM_SRCS  = src/gpu/metal_bench.mm

# OBJECTS
CPP_OBJS = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CPP_SRCS))
MM_OBJS  = $(patsubst %.mm,  $(BUILDDIR)/%.o, $(MM_SRCS))
OBJS     = $(CPP_OBJS) $(MM_OBJS)


.PHONY: all clean rebuild

all: $(TARGET)

# LINK 
$(TARGET): $(OBJS)
	@echo "  LINK     $@"
	@$(OBJCXX) $(OBJS) $(LDFLAGS) -o $@
	@echo "  DONE     ./$@"

#  COMPILE .cpp
$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX      $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

#  COMPILE .mm
$(BUILDDIR)/%.o: %.mm
	@mkdir -p $(dir $@)
	@echo "  OBJCXX   $<"
	@$(OBJCXX) $(MMFLAGS) -c $< -o $@

# CLEAN 
clean:
	@rm -rf $(BUILDDIR) $(TARGET)
	@echo "  CLEAN    done"

rebuild: clean all