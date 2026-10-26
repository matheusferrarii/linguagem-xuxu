CXX      = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror
SRCDIR   = src

BIN_REGEX = $(SRCDIR)/02_demo
BIN_AFD   = $(SRCDIR)/afd

SRCS_REGEX = $(SRCDIR)/02_demo.cpp $(SRCDIR)/02_regex.cpp
SRCS_AFD   = $(SRCDIR)/afd.cpp

all: $(BIN_REGEX) $(BIN_AFD)

$(BIN_REGEX): $(SRCS_REGEX) $(SRCDIR)/02_regex.h
	$(CXX) $(CXXFLAGS) -o $(BIN_REGEX) $(SRCS_REGEX)

$(BIN_AFD): $(SRCS_AFD)
	$(CXX) $(CXXFLAGS) -o $(BIN_AFD) $(SRCS_AFD)

run: all
	$(BIN_REGEX)
	$(BIN_AFD)

clean:
	rm -f $(BIN_REGEX) $(BIN_AFD)

.PHONY: all run clean
