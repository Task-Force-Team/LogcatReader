# Target executable name
TARGET = logcatreader

# Source files
SRCS = main.cpp LogcatReader.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 `pkg-config --cflags gtkmm-3.0`

# Linker flags
LDFLAGS = `pkg-config --libs gtkmm-3.0`

# Default target
all: $(TARGET)

# Rule to link the object files into the executable
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

install:
    install -Dm755 logcatreader $(DESTDIR)/usr/bin/logcatreader
