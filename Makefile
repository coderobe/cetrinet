CC:=g++
LD:=g++

CFLAGS+=-pthread -Og -std=c++17
LDFLAGS+=

INCLUDE = -Iinclude
LIBS = -Llib -lLCUI -lLCUIEx -lssl -lcrypto -lboost_system

SOURCES=src/cetrinet.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OUTDIR:=bin
TARGET:=${OUTDIR}/cetrinet

.DEFAULT_GOAL=all

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@ ${LIBS}

${TARGET}: ${OBJECTS}
	mkdir -p $(dir ${TARGET})
	${LD} ${LDFLAGS} ${CFLAGS} -o $@ $+ ${LIBS}

run: ${TARGET}
	(cd bin && ./$(notdir ${TARGET}))

.PHONY: all build debug

all: build run

assets:
	cp src/*.xml src/*.css src/*.ttf ${OUTDIR}

build: ${TARGET} assets

debug: CFLAGS+=-ggdb -D_DEBUG
debug: build

clean:
	rm -f ${TARGET} ${OBJECTS}
