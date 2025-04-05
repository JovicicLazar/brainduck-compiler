# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Directory
DIR = src/

# Source files
SRCS = main.cpp $(DIR)compiler.cpp $(DIR)tokenizer.cpp $(DIR)utils.cpp
OBJS = $(SRCS:.cpp=.o)

# Output binary
TARGET = brain

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
