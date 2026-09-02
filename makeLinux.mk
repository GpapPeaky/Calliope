# 64-bit Linux g++
CXX = g++
TARGET = cblt

# Release shared library
RELEASE_DIR = release
DLL_TARGET  = libCoBaLT.so
DLL_LDFLAGS = -shared -Wl,--export-dynamic

# Raylib path
RAYLIB_PATH = /home/peaky/Desktop/GpapPeaky/dev/lib/raylib/src/

# Compiler flags
CXXFLAGS = -std=c++23 -fPIC -Wall -Wextra -O3 -I$(RAYLIB_PATH)

# Common source files
CPP_SRCS_COMMON := $(filter-out CBLT_Dialog_Win32.cpp CBLT_Dialog_Linux.cpp CBLT_Dialog_MacOS.cpp, $(wildcard *.cpp))

# Platform-specific source
CPP_SRCS_PLATFORM := CBLT_Dialog_Linux.cpp
CPP_SRCS := $(CPP_SRCS_COMMON) $(CPP_SRCS_PLATFORM)

OBJDIR := obj
CPP_OBJS := $(CPP_SRCS:%.cpp=$(OBJDIR)/%.o)
OBJS := $(CPP_OBJS)

# Linker flags for Linux
LDFLAGS := -L$(RAYLIB_PATH) -l:libraylib.a -lGL -lm -ldl -lpthread -lrt \
		-lX11 -lXrandr -lXi -lXxf86vm -lXinerama -lXcursor 				\
		-static-libstdc++ -static-libgcc

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	mkdir -p $(OBJDIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile object files
$(OBJDIR)/%.o: %.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build shared library
$(RELEASE_DIR)/$(DLL_TARGET): $(OBJS)
	mkdir -p $(RELEASE_DIR)
	$(CXX) $(OBJS) $(DLL_LDFLAGS) -o $@ $(LDFLAGS)

# Clean
clean:
	rm -rf $(OBJDIR) $(TARGET) $(RELEASE_DIR)

# Release target
rel: clean $(RELEASE_DIR)/$(DLL_TARGET)