CXX = g++
TARGET = CoBaLT.exe

# Path to your locally compiled raylib
RAYLIB_PATH = C:/raylib/raylib/src   # adjust if needed

# Compiler and linker flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -I$(RAYLIB_PATH)
LDFLAGS  = -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm -lcomdlg32 -lole32

# Source files and object folder
CPP_SRCS = $(wildcard *.cpp)
C_SRCS   = CBLT_tinyfiledialogs.c

OBJDIR = obj
CPP_OBJS = $(CPP_SRCS:%.cpp=$(OBJDIR)/%.o)
C_OBJS   = $(C_SRCS:%.c=$(OBJDIR)/%.o)

OBJS = $(CPP_OBJS) $(C_OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
