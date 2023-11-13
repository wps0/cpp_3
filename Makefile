CXXFLAGS = -Wall -Wextra -O2 -std=c++20
BINARIES = crosswords crosswords_example

all: $(BINARIES)

crosswords: crosswords.cc
	g++ $(CXXFLAGS) *.cc

crosswords.o: crosswords.cpp
crosswords_example.o: crosswords_example.cc

crosswords_example: crosswords.o crosswords_example.o
	g++ $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(BINARIES) *.o

.PHONY: clean all

