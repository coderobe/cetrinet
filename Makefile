CXX:=clang++

TARGET=build/cetrinet

.PHONY: all build release clean

all: build/cetrinet

build/cetrinet: build/build.ninja $(shell meson/wildcard 'src/**/*')
	ninja -C build 2>&1 | sed -u -E 's/\.\.\/(src\/)/\1/'

release: ${TARGET}
	strip ${TARGET}
	upx --best ${TARGET}

clean:
	rm -rf build assets


build/build.ninja:
	CXX=${CXX} meson . build

assets/main.xml:
	meson/assets assets
