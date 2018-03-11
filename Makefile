CXX:=clang++

TARGET=build/cetrinet

.PHONY: all build release release-windows clean

all: ${TARGET}

${TARGET}: build/build.ninja $(shell meson/wildcard 'src/**/*')
	ninja -C build 2>&1 | sed -u -E 's/\.\.\/(src\/)/\1/'

win${TARGET}.exe: winbuild/build.ninja $(shell meson/wildcard 'src/**/*')
	ninja -C winbuild

release: ${TARGET}
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag ${TARGET}
	sstrip -z ${TARGET}
	upx --ultra-brute --overlay=strip ${TARGET}

release-windows: win${TARGET}.exe
	upx --ultra-brute --overlay=strip --strip-relocs=0 win${TARGET}.exe

clean:
	rm -rf build winbuild

build/build.ninja:
	CXX=${CXX} meson . build

winbuild/build.ninja:
	meson --cross-file windows.xcc . winbuild
