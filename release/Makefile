CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall
LDFLAGS = -lGLEW -lGL -lSDL2 -lm
SRCDIR = src
OBJDIR = obj
BINDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
TARGET = $(BINDIR)/voxel-gl
all: dirs $(TARGET)
dirs:
	@mkdir -p $(OBJDIR) $(BINDIR)
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm -rf $(OBJDIR) $(BINDIR)
run: all
	./$(TARGET)
.PHONY: all clean run dirs
