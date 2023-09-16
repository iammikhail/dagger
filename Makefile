CXX = g++
CXXFLAGS = -std=c++20 -Wall -MMD -I.

ifdef DEBUG
ifeq (${DEBUG}, 1)
CXXFLAGS += -g -DDEBUG
endif
endif

TARGET_DIR = bin
BUILD_DIR = build

TARGET = main
SOURCES = main.cpp $(shell find dagger -type f -name "*.cpp")
$(info $(SOURCES))
# OBJECTS = $(patsubst dagger/%,build/%,${SOURCES:.cpp=.o})
OBJECTS = ${addprefix ${BUILD_DIR}/,${SOURCES:.cpp=.o}}
$(info $(OBJECTS))
DEPENDS = ${OBJECTS:.o=.d}
LIB = -pthread

# all: directories ${TARGET}

# directories:
# 	@mkdir -p bin
# 	@mkdir -p build

${TARGET}: ${OBJECTS}
	@mkdir -p ${TARGET_DIR}
	${CXX} ${OBJECTS} ${LIB} -o ${TARGET_DIR}/${TARGET}

-include ${DEPENDS}

${BUILD_DIR}/%.o: %.cpp
	@mkdir -p ${@D}
	${CXX} ${CXXFLAGS} -c -o $@ $<

.PHONY: clean

clean:
	rm -rf ${TARGET_DIR}
	rm -rf ${BUILD_DIR}
