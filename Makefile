CXX      = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror
SRCDIR   = src

BIN_REGEX = $(SRCDIR)/02_demo
BIN_AFD   = $(SRCDIR)/afd
BIN_AFN   = $(SRCDIR)/04_demo
BIN_DET   = $(SRCDIR)/05_demo

SRCS_REGEX = $(SRCDIR)/02_demo.cpp $(SRCDIR)/02_regex.cpp
SRCS_AFD   = $(SRCDIR)/afd.cpp
SRCS_AFN   = $(SRCDIR)/04_demo.cpp $(SRCDIR)/04_afn.cpp $(SRCDIR)/02_regex.cpp
SRCS_DET   = $(SRCDIR)/05_demo.cpp $(SRCDIR)/05_determinizacao.cpp $(SRCDIR)/04_afn.cpp $(SRCDIR)/02_regex.cpp

all: $(BIN_REGEX) $(BIN_AFD) $(BIN_AFN) $(BIN_DET)

$(BIN_REGEX): $(SRCS_REGEX) $(SRCDIR)/02_regex.h
	$(CXX) $(CXXFLAGS) -o $(BIN_REGEX) $(SRCS_REGEX)

$(BIN_AFD): $(SRCS_AFD)
	$(CXX) $(CXXFLAGS) -o $(BIN_AFD) $(SRCS_AFD)

$(BIN_AFN): $(SRCS_AFN) $(SRCDIR)/02_regex.h $(SRCDIR)/04_afn.h
	$(CXX) $(CXXFLAGS) -o $(BIN_AFN) $(SRCS_AFN)

$(BIN_DET): $(SRCS_DET) $(SRCDIR)/02_regex.h $(SRCDIR)/04_afn.h $(SRCDIR)/05_determinizacao.h
	$(CXX) $(CXXFLAGS) -o $(BIN_DET) $(SRCS_DET)

afn: $(BIN_AFN)
	$(BIN_AFN) afn

determinizar: $(BIN_DET)
	$(BIN_DET) determinizar

run: all
	$(BIN_REGEX)
	$(BIN_AFD)
	$(BIN_AFN) afn
	$(BIN_DET) determinizar

clean:
	rm -f $(BIN_REGEX) $(BIN_AFD) $(BIN_AFN) $(BIN_DET)

.PHONY: all run clean afn determinizar
