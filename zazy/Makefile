CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wno-unknown-pragmas
SRC       = src/Main.cpp src/Lexer/Lexer.cpp

all: zazy tools/z80run

zazy: $(SRC) $(wildcard src/*.h src/*/*.h)
	$(CXX) $(CXXFLAGS) -I. $(SRC) -o $@

# headless Z80 used by the tests only
tools/z80run: tools/z80run.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# ZAZY_ASM=/path/to/your/assembler make test      (called as: ASM in.z80 out.bin out.sym)
test: all
	python3 tests/run.py
	python3 tests/fuzz.py -n 100
	python3 tests/fuzz.py --w8 -n 100

clean:
	rm -f zazy tools/z80run

.PHONY: all test clean
