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

ifeq ($(type), lib)
	APP = ysl.so
	CXXLIBS = -fPIC
endif

# compiler related
CXXVER = c++17
CXXFLAGS += \
	-O0 \
	-std=${CXXVER} \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-ggdb

CXXLIBS +=

# rules
compile: ./bin ${OBJ} ${SRC}
	${CCACHE} ${CXX} -o ${APP} ${OBJ} ${CXXLIBS}

./bin:
	mkdir -p bin

bin/%.o: src/%.cc
	${CCACHE} ${CXX} -c $< ${CXXFLAGS} -DYSL_VERSION='$(VER)' -o $@

clean:
	rm bin/*.o $(APP)

install:
	cp $(APP) /usr/bin/

all:
	@echo compile
	@echo clean
	@echo install
