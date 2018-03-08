CXX:=clang++

TARGET=build/cetrinet

.PHONY: all build release clean

all: build/cetrinet

build/cetrinet: build/build.ninja assets/main.xml $(shell meson/wildcard 'src/**/*')
	ninja -C build 2>&1 | sed -u -E 's/\.\.\/(src\/)/\1/'

release: ${TARGET}
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag ${TARGET}
	upx --brute ${TARGET}

clean:
	rm -rf build assets


build/build.ninja:
	CXX=${CXX} meson . build

assets/main.xml: src/main.xml
	meson/assets assets
