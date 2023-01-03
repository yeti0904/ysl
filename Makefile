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
	CXXSHARED = -shared
	CXXFLAGS += -fPIC -DYSL_ISLIB -DYSL_NO_GRAPHICS
else ifeq ($(graphics), off)
	CXXFLAGS += DYSL_NO_GRAPHICS
else
	CXXLIBS += -lraylib -lpthread -lGL -lm -ldl -lrt -lX11
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

# rules
compile: ./bin ${OBJ} ${SRC}
	${CCACHE} ${CXX} ${CXXSHARED} -o ${APP} ${OBJ} ${CXXLIBS}

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
