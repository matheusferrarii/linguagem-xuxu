CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror
SRCDIR   = src
BIN      = $(SRCDIR)/02_demo

SRCS = $(SRCDIR)/02_demo.cpp $(SRCDIR)/02_regex.cpp

all: $(BIN)

$(BIN): $(SRCS) $(SRCDIR)/02_regex.h
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRCS)

run: $(BIN)
	$(BIN)

clean:
	rm -f $(BIN)
