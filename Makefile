CC:=clang++
LD:=clang++

CFLAGS+=-pthread -Os -std=c++17
LDFLAGS+=

INCLUDE = -Iinclude
LIBS = -Llib -lssl -lcrypto -lxml2 -lfreetype -lpng -ljpeg -lX11
LIBS += -Wl,-Bstatic -lboost_system -lLCUI -lLCUIEx -Wl,-Bdynamic

SOURCES=$(wildcard src/*.cpp) $(wildcard src/proto/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
OUTDIR:=bin
TARGET:=${OUTDIR}/cetrinet

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@

${TARGET}: ${OBJECTS} assets/main.xml
	mkdir -p $(dir ${TARGET})
	${LD} ${LDFLAGS} ${CFLAGS} -o $@ $(filter-out assets/main.xml,$+) ${LIBS}

run: ${TARGET}
	(cd bin && ./$(notdir ${TARGET}))

.PHONY: all build debug

all: clean build run

assets/main.xml: assets
assets: src/*.css src/*.xml
	mkdir -p assets
	cp src/*.css assets
	cp src/*.xml assets
	minify src/main.xml > assets/main.xml

pack: build ${TARGET}
	upx --best ${TARGET}

build: assets ${TARGET}
release: build pack

debug: CFLAGS+=-ggdb -D_DEBUG -Og
debug: build

clean:
	rm -f ${TARGET} ${OBJECTS}
	rm -rf assets
