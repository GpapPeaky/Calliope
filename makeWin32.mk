# For god's sake do not compile ANYTHING inside Git-bash, fucks up PATH, STDLIB and appearantly C Runtime and DLL comp.

# 64-bit MinGW
CXX = x86_64-w64-mingw32-g++
TARGET = CoBaLT.exe

# Release DLL
RELEASE_DIR = release
DLL_TARGET  = CoBaLT.dll
IMPLIB      = libCoBaLT.dll.a
DLL_LDFLAGS = -static -shared -Wl,--export-all-symbols -Wl,--out-implib,$(RELEASE_DIR)/$(IMPLIB)

# Raylib path
RAYLIB_PATH = C:/raylib/raylib/src

# Compiler flags
CXXFLAGS = -std=c++23 -Wall -Wextra -O3 -I$(RAYLIB_PATH)

# Common source files
CPP_SRCS_COMMON := $(filter-out CBLT_Dialog_Win32.cpp CBLT_Dialog_Linux.cpp CBLT_Dialog_MacOS.cpp, $(wildcard *.cpp))

CPP_SRCS_PLATFORM := CBLT_Dialog_Win32.cpp
LDFLAGS := -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm -lole32 -lshell32 -luuid -static-libgcc -static-libstdc++

CPP_SRCS := $(CPP_SRCS_COMMON) $(CPP_SRCS_PLATFORM)
OBJDIR := obj
CPP_OBJS := $(CPP_SRCS:%.cpp=$(OBJDIR)/%.o)
OBJS := $(CPP_OBJS)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile object files
$(OBJDIR)/%.o: %.cpp
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build DLL
$(RELEASE_DIR)/$(DLL_TARGET): $(OBJS)
	@if not exist $(RELEASE_DIR) mkdir $(RELEASE_DIR)
	$(CXX) $(OBJS) $(DLL_LDFLAGS) -o $@ $(LDFLAGS)

# Clean
clean:
	@if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
	@if exist $(TARGET) del /q $(TARGET)
	@if exist $(RELEASE_DIR) rmdir /s /q $(RELEASE_DIR)

# Release target
rel: clean $(RELEASE_DIR)/$(DLL_TARGET)
