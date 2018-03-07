CXX:=clang++

TARGET=build/cetrinet

.PHONY: all build pack clean

all: build

build: build/build.ninja
	(cd build && ninja)

pack: ${TARGET}
	upx --best ${TARGET}

clean:
	rm -rf build assets


build/build.ninja:
	CXX=${CXX} meson . build

assets/main.xml:
	meson/assets assets
