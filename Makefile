# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Source files
SRCS = main.cpp compiler.cpp tokenizer.cpp
OBJS = $(SRCS:.cpp=.o)

# Output binary
TARGET = main

# Default target
all: $(TARGET)

# Compile the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean compiled files
clean:
	rm -f $(OBJS) $(TARGET)
