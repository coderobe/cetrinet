CC:=g++
LD:=g++

CFLAGS+=-pthread -O3 -std=c++17
LDFLAGS+=

INCLUDE = -Iinclude
LIBS = -Llib -lssl -lcrypto -lxml2 -lfreetype -lpng -ljpeg -lX11
LIBS += -Wl,-Bstatic -lboost_system -lLCUI -lLCUIEx -Wl,-Bdynamic

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

all: clean build run

assets: src/*.css src/*.xml
	mkdir -p assets
	#cp src/*.ttf assets
	cp src/*.css assets
	cp src/*.xml assets
	minify src/main.xml > assets/main.xml

pack: build ${TARGET}
	upx --best ${TARGET}

build: assets ${TARGET}
release: clean build pack

debug: CFLAGS+=-ggdb -D_DEBUG
debug: build

clean:
	rm -f ${TARGET} ${OBJECTS}
