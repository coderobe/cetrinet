CC:=clang++
LD:=clang++

CFLAGS+=-pthread -Os -std=c++17
LDFLAGS+=

INCLUDE = -Iinclude
LIBS = -Llib -lssl -lcrypto -lxml2 -lfreetype -lpng -ljpeg -lX11
LIBS += -Wl,-Bstatic -lboost_system -lLCUI -lLCUIEx -Wl,-Bdynamic

SOURCES=src/cetrinet.cpp src/util.cpp src/net.cpp src/ui.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OUTDIR:=bin
TARGET:=${OUTDIR}/cetrinet

.DEFAULT_GOAL=all

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@

${TARGET}: ${OBJECTS}
	mkdir -p $(dir ${TARGET})
	${LD} ${LDFLAGS} ${CFLAGS} -o $@ $+ ${LIBS}

run: ${TARGET}
	(cd bin && ./$(notdir ${TARGET}))

.PHONY: all build debug

all: clean build run

assets: src/*.css src/*.xml
	mkdir -p assets
	cp src/*.css assets
	cp src/*.xml assets
	minify src/main.xml > assets/main.xml

pack: build ${TARGET}
	upx --best ${TARGET}

build: assets ${TARGET}
release: clean build pack

debug: CFLAGS+=-ggdb -D_DEBUG -Og
debug: build

clean:
	rm -f ${TARGET} ${OBJECTS}
	rm -rf assets
