CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
SRC := src/main.cpp
BIN := simulador_cache

all: $(BIN)

$(BIN): $(SRC) src/cache.hpp src/filesystem_profile.hpp src/trace_generator.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
