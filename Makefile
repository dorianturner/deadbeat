CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude
LDFLAGS = -lportaudio -lm

SRC = src/main.cpp src/ast.cpp src/parser.cpp src/interpreter.cpp src/audio.cpp src/utils.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = deadbeat

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
