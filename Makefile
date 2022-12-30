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
DEPS += ${wildcard src/extensions/*}
OBJ   = ${addsuffix .o,${subst src/,bin/,${basename ${SRC}}}}

APP = ysl

ifeq ($(type), lib)
	APP = libysl.so
	CXXLIBS = -shared -fPIC
	CXXFLAGS += -DYSL_ISLIB
endif

# compiler related
CXXVER = c++17
CXXFLAGS += \
	-std=${CXXVER} \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-fms-extensions

ifeq ($(mode), release)
	CXXFLAGS += -Ofast
else
	CXXFLAGS += -O0 -ggdb
endif

CXXLIBS += -lraylib

# rules
compile: ./bin ${OBJ} ${SRC}
	${CCACHE} ${CXX} ${CXXLIBS} -o ${APP} ${OBJ}

./bin:
	mkdir -p bin

bin/%.o: src/%.cc ${DEPS}
	${CCACHE} ${CXX} -c $< ${CXXFLAGS} -DYSL_VERSION='$(VER)' -o $@

clean:
	rm bin/*.o $(APP)

install:
	cp $(APP) /usr/bin/

all:
	@echo compile
	@echo clean
	@echo install
