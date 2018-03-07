CXX:=clang++

TARGET=build/cetrinet

.PHONY: all build release clean

all: build/cetrinet

build/cetrinet: build/build.ninja $(shell meson/wildcard 'src/**/*')
	(cd build && ninja)

release: ${TARGET}
	strip ${TARGET}
	upx --best ${TARGET}

clean:
	rm -rf build assets


build/build.ninja:
	CXX=${CXX} meson . build

assets/main.xml:
	meson/assets assets
