CXX = g++
TARGET = CoBaLT.exe

# Path to your locally compiled raylib
RAYLIB_PATH = C:/raylib/raylib/src   # adjust if needed

# Compiler and linker flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I$(RAYLIB_PATH)
LDFLAGS  = -L$(RAYLIB_PATH) -lraylib -lopengl32 -lgdi32 -lwinmm

# Source files and object folder
SRCS   = $(wildcard *.cpp)
OBJDIR = obj
OBJS   = $(SRCS:%.cpp=$(OBJDIR)/%.o)

# Target
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .cpp -> obj/*.o
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJDIR) $(TARGET)
