CXX      = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror
SRCDIR   = src

BIN_REGEX = $(SRCDIR)/02_demo
BIN_AFD   = $(SRCDIR)/afd
BIN_AFN   = $(SRCDIR)/04_demo

SRCS_REGEX = $(SRCDIR)/02_demo.cpp $(SRCDIR)/02_regex.cpp
SRCS_AFD   = $(SRCDIR)/afd.cpp
SRCS_AFN   = $(SRCDIR)/04_demo.cpp $(SRCDIR)/04_afn.cpp $(SRCDIR)/02_regex.cpp

all: $(BIN_REGEX) $(BIN_AFD) $(BIN_AFN)

$(BIN_REGEX): $(SRCS_REGEX) $(SRCDIR)/02_regex.h
	$(CXX) $(CXXFLAGS) -o $(BIN_REGEX) $(SRCS_REGEX)

$(BIN_AFD): $(SRCS_AFD)
	$(CXX) $(CXXFLAGS) -o $(BIN_AFD) $(SRCS_AFD)

$(BIN_AFN): $(SRCS_AFN) $(SRCDIR)/02_regex.h $(SRCDIR)/04_afn.h
	$(CXX) $(CXXFLAGS) -o $(BIN_AFN) $(SRCS_AFN)

afn: $(BIN_AFN)
	$(BIN_AFN) afn

run: all
	$(BIN_REGEX)
	$(BIN_AFD)
	$(BIN_AFN) afn

clean:
	rm -f $(BIN_REGEX) $(BIN_AFD) $(BIN_AFN)

.PHONY: all run clean afn
