CXX := $(shell command -v clang++ 2> /dev/null)
ifndef CXX
CXX = g++
endif

CCACHE := $(shell command -v ccache 2> /dev/null)
ifndef CXX
CCACHE = ""
endif

VER = $(shell git log -1 --pretty=format:"\"%H\"")

# files
SRC   = ${wildcard src/*.cc}
DEPS += ${wildcard src/*.hh}
OBJ   = ${addsuffix .o,${subst src/,bin/,${basename ${SRC}}}}

APP = ysl

# compiler related
CXXVER = c++17
CXXFLAGS = \
	-O0 \
	-std=${CXXVER} \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-ggdb

# rules
compile: ./bin ${OBJ} ${SRC}
	${CCACHE} ${CXX} -o ${APP} ${OBJ}

./bin:
	mkdir -p bin

bin/%.o: src/%.cc
	${CCACHE} ${CXX} -c $< ${CXXFLAGS} ${CXXLIBS} -DYSL_VERSION='$(VER)' -o $@

clean:
	rm bin/*.o $(APP)

install:
	cp $(APP) /usr/bin/

all:
	@echo compile
	@echo clean
	@echo install
