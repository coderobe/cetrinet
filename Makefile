CXX:=clang++

TARGET=build/cetrinet

.PHONY: all build release clean

all: build

build/cetrinet: build/build.ninja
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
