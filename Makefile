CXX := $(shell command -v clang++ 2> /dev/null)
ifndef CXX
CXX = g++
endif

VER = $(shell git log -1 --pretty=format:"\"%H\"")

SRC = interpreter.cc
OUT = ysl

build:
	$(CXX) $(SRC) -DYSL_VERSION='$(VER)' -o $(OUT)
