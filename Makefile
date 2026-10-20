CXX = g++
CXXFLAGS = -std=c++17 -Wall -Isrc

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,build/%.o,$(SRCS))
TARGET = potish

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)

.PHONY: clean
