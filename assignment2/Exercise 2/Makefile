# C++ compiler (override on target systems, e.g. make CXX=CC on Dardel)
CXX ?= g++

# Include directory
INCLUDES = -I../include

# Compiler flags
CXXFLAGS ?= -O3 -fopenmp -march=native -funroll-loops

# Output executable name
TARGET = roofline

# Source files
SRCS = simple_roofline.cpp

# Object files (automatically generated from SRCS)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
