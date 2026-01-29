CXX = g++
TARGET = CoBaLT.exe

# Path to your locally compiled raylib
RAYLIB_PATH = C:/raylib/raylib/src   # ADJUST IF NEEDED!

# Compiler and linker flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -I$(RAYLIB_PATH)

# Common source files
CPP_SRCS_COMMON := $(filter-out CBLT_Dialog_Win32.cpp CBLT_Dialog_Linux.cpp CBLT_Dialog_MacOS.cpp, $(wildcard *.cpp))

# Platform-specific sources
ifeq ($(OS),Windows_NT)
    CPP_SRCS_PLATFORM := CBLT_Dialog_Win32.cpp
    LDFLAGS := -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CPP_SRCS_PLATFORM := CBLT_Dialog_Linux.cpp
        LDFLAGS := -L$(RAYLIB_PATH) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    ifeq ($(UNAME_S),Darwin)
        CPP_SRCS_PLATFORM := CBLT_Dialog_MacOS.cpp
        LDFLAGS := -L$(RAYLIB_PATH) -lraylib \
                   -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    endif
endif

# All sources
CPP_SRCS := $(CPP_SRCS_COMMON) $(CPP_SRCS_PLATFORM)

# Objects
OBJDIR := obj
CPP_OBJS := $(CPP_SRCS:%.cpp=$(OBJDIR)/%.o)
OBJS := $(CPP_OBJS)


$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(OBJDIR)/%.o: %.c
# 	@mkdir -p $(OBJDIR)
# 	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
